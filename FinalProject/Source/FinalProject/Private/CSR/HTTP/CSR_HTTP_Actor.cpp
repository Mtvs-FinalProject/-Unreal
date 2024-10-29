// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/HTTP/CSR_HTTP_Actor.h"
#include "Kismet/GameplayStatics.h"
#include "PSH/PSH_Player/PSH_Player.h"
#include "Templates/SharedPointer.h"
#include "PSH/PSH_Actor/PSH_BlockActor.h"
#include "HttpModule.h"


// Sets default values
ACSR_HTTP_Actor::ACSR_HTTP_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ACSR_HTTP_Actor::BeginPlay()
{
	Super::BeginPlay();
	
}

TArray<uint8> ACSR_HTTP_Actor::LoadUMapFile(const FString& FilePath)
{
    TArray<uint8> FileData;
    if (FFileHelper::LoadFileToArray(FileData, *FilePath))
    {
        UE_LOG(LogTemp, Log, TEXT("UMap file loaded successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load UMap file"));
    }
    return FileData;
}

FString ACSR_HTTP_Actor::ConvertStructToJsonString(const FPSH_ObjectData& ObjectData)
{
    FString JsonString;
    if (FJsonObjectConverter::UStructToJsonObjectString(ObjectData, JsonString))
    {
        UE_LOG(LogTemp, Log, TEXT("Struct converted to JSON successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to convert struct to JSON"));
    }
    return JsonString;
}

TArray<uint8> ACSR_HTTP_Actor::CreateMultipartData(const TArray<uint8>& FileData, const FString& JsonString)
{
    TArray<uint8> Content;
    FString BeginBoundary = "--" + BOUNDARY + "\r\n";
    FString EndBoundary = "\r\n--" + BOUNDARY + "--\r\n";

    // JSON 파트 추가 (필드 이름을 json_data로 변경)
    FString JsonPartHeader = BeginBoundary +
        "Content-Disposition: form-data; name=\"data\"\r\n\r\n" + // 수정된 부분
        JsonString + "\r\n";
    Content.Append((uint8*)TCHAR_TO_UTF8(*JsonPartHeader), JsonPartHeader.Len());

    // 파일 파트 추가
    FString FilePartHeader = BeginBoundary +
        "Content-Disposition: form-data; name=\"file\"; filename=\"YourMap.umap\"\r\n" +
        "Content-Type: application/octet-stream\r\n\r\n";
    Content.Append((uint8*)TCHAR_TO_UTF8(*FilePartHeader), FilePartHeader.Len());
    Content.Append(FileData);
    Content.Append((uint8*)TCHAR_TO_UTF8(*EndBoundary), EndBoundary.Len());

    return Content;
}

void ACSR_HTTP_Actor::SendUMapAndJsonToServer()
{
    UE_LOG(LogTemp, Warning, TEXT("hellp"));
    APSH_Player* player = Cast<APSH_Player>(this->GetWorld()->GetFirstPlayerController()->GetCharacter());

    if (!player || !player->dataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("Player or data table not found."));
        return;
    }


    //if (!data)
    //{
    //    UE_LOG(LogTemp, Error, TEXT("Row not found in data table."));
    //    return;
    //}

    if (!FPaths::FileExists(UMAP_FILE_PATH))
    {
        UE_LOG(LogTemp, Error, TEXT("File does not exist at path: %s"), *UMAP_FILE_PATH);
        return;
    }

    TArray<uint8> FileData = LoadUMapFile(UMAP_FILE_PATH);
    if (FileData.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("No UMap file data to send."));
        return;
    }
    FString DataTableJsonString = player->dataTable->GetTableAsJSON();
    TSharedPtr<FJsonObject> RootJsonObject = MakeShareable(new FJsonObject);

    RootJsonObject->SetNumberField(TEXT("price"), 0);
    RootJsonObject->SetStringField(TEXT("editable"), TEXT("ABLE"));
    RootJsonObject->SetNumberField(TEXT("uploader"), 0);
    TArray<TSharedPtr<FJsonValue>> DataTableArray;
    TSharedRef<TJsonReader<>> DataTableReader = TJsonReaderFactory<>::Create(DataTableJsonString);
    TArray<TSharedPtr<FJsonValue>> ParsedDataTableArray;

    if (FJsonSerializer::Deserialize(DataTableReader, ParsedDataTableArray))
    {
        for (auto& Entry : ParsedDataTableArray)
        {
            DataTableArray.Add(Entry);
        }
        RootJsonObject->SetArrayField(TEXT("data_table"), DataTableArray);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse DataTable JSON."));
        return;
    }

    // 최종 JSON 문자열 생성
    FString FinalJsonString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&FinalJsonString);
    FJsonSerializer::Serialize(RootJsonObject.ToSharedRef(), Writer);

    UE_LOG(LogTemp, Warning, TEXT("Final JSON to send: %s"), *FinalJsonString);
    Req_SaveUMapAndJson(FileData, FinalJsonString);
}

void ACSR_HTTP_Actor::Req_SaveUMapAndJson(const TArray<uint8>& FileData, const FString& JsonString)
{
    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(SERVERSAVE_URL);
    Request->SetVerb("POST");
    Request->SetHeader(TEXT("Content-Type"), FString::Printf(TEXT("multipart/form-data; boundary=%s"), *BOUNDARY));

    // 멀티파트 데이터 설정
    TArray<uint8> Content = CreateMultipartData(FileData, JsonString);
    Request->SetContent(Content);

    // 응답 처리 델리게이트 설정
    Request->OnProcessRequestComplete().BindUObject(this, &ACSR_HTTP_Actor::Res_SaveUMapAndJson);

    Request->ProcessRequest();
}

void ACSR_HTTP_Actor::Res_SaveUMapAndJson(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response->GetResponseCode() == 200)
    {
        UE_LOG(LogTemp, Log, TEXT("UMap and JSON sent successfully: %s"), *Response->GetContentAsString());

        // 게임 재시작
        UWorld* World = GetWorld();
        if (World)
        {
            FName CurrentLevelName = *World->GetMapName(); // 현재 레벨 이름 가져오기
            UGameplayStatics::OpenLevel(World, CurrentLevelName); // 현재 레벨 다시 열기
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to send UMap and JSON: %d"), Response->GetResponseCode());
        if (Response.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("Response: %s"), *Response->GetContentAsString());
        }
    }
}

void ACSR_HTTP_Actor::Req_DownMap()
{
    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(SERVERDOWN_URL); // FastAPI 서버의 다운로드 엔드포인트 설정
    Request->SetVerb("GET");

    // 응답 처리 델리게이트 설정
    Request->OnProcessRequestComplete().BindUObject(this, &ACSR_HTTP_Actor::Res_DownMap);

    // 요청 전송
    Request->ProcessRequest();
}

void ACSR_HTTP_Actor::Res_DownMap(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to receive response from server."));
        return;
    }

    FString ResponseContent = Response->GetContentAsString();
    FString Boundary = "--"; // boundary는 서버 응답의 Content-Type 헤더에서 얻을 수 있습니다.

    // Boundary를 기준으로 파트를 분해
    TArray<FString> Parts;
    ResponseContent.ParseIntoArray(Parts, *Boundary, true);

    FString JsonContent;

    // JSON 파트를 추출
    for (const FString& Part : Parts)
    {
        if (Part.Contains(TEXT("Content-Type: application/json")))
        {
            int32 ContentIndex = Part.Find(TEXT("{")); // JSON 데이터 시작 위치 찾기
            if (ContentIndex != INDEX_NONE)
            {
                JsonContent = Part.Mid(ContentIndex); // JSON 데이터 추출
                UE_LOG(LogTemp, Log, TEXT("Extracted JSON: %s"), *JsonContent);
                break; // JSON 파트를 찾았으므로 루프 종료
            }
        }
    }

    FString CleanedJsonContent = JsonContent.Replace(TEXT(" "), TEXT("")).Replace(TEXT("\n"), TEXT("")).Replace(TEXT("\r"), TEXT(""));

    // JSON에서 "data_table" 필드를 TArray<FPSH_ObjectData>로 파싱
    if (!JsonContent.IsEmpty())
    {
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(CleanedJsonContent);

        if (FJsonSerializer::Deserialize(Reader, JsonObject))
        {
            if (JsonObject->HasField("data_table"))
            {
                // data_table 필드를 TArray<FPSH_ObjectData>로 파싱
                TArray<FPSH_ObjectData> DataTableArray;
                const TArray<TSharedPtr<FJsonValue>>* JsonArray;

                if (JsonObject->TryGetArrayField("data_table", JsonArray))
                {
                    for (const TSharedPtr<FJsonValue>& Value : *JsonArray)
                    {
                        FPSH_ObjectData ObjectData;
                        if (FJsonObjectConverter::JsonObjectToUStruct(Value->AsObject().ToSharedRef(), &ObjectData))
                        {
                            DataTableArray.Add(ObjectData);
                        }
                    }

                    // DataTableArray의 데이터로 블록 스폰
                    for (const FPSH_ObjectData& Entry : DataTableArray)
                    {
                        // 루트 블록 스폰
                        TSubclassOf<APSH_BlockActor> SpawnActor = Entry.actor;
                        if (SpawnActor)
                        {
                            FActorSpawnParameters Params;
                            FVector SpawnLocation = GetActorLocation(); // 기본 위치 또는 적절한 오프셋
                            FRotator SpawnRotation = FRotator::ZeroRotator;

                            APSH_BlockActor* SpawnedBlock = GetWorld()->SpawnActor<APSH_BlockActor>(SpawnActor, SpawnLocation, SpawnRotation, Params);
                            if (SpawnedBlock)
                            {
                                // 블록 계층 로드
                                SpawnedBlock->LoadBlockHierarchy(Entry);
                            }
                        }
                    }
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("data_table field not found in JSON response."));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response."));
        }
    }
}