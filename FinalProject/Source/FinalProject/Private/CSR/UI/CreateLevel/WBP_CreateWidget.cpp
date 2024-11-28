// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/CreateLevel/WBP_CreateWidget.h"
#include "PSH/PSH_DataTable/PSH_MechDataTable.h"
#include "JsonObjectConverter.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "Policies/PrettyJsonPrintPolicy.h"
#include "CSR/Auth/AuthSubsystem.h"
#include "CSR/DedicatedServer/AutoRoomLevelInstance.h"
#include "PSH/PSH_Player/PSH_PlayerController.h"

void UWBP_CreateWidget::NativeConstruct()
{
	this->WBP_Click_Zone->SaveButtonClick.AddDynamic(this, &UWBP_CreateWidget::CreateMapHTTP);
	this->WBP_Click_Zone->CancleButtonClick.AddDynamic(this, &UWBP_CreateWidget::CancleThisUI);
}

void UWBP_CreateWidget::CreateMapHTTP()
{
    // 기존의 JSON 생성
    FString JsonString = CreateBaseJson();

    // 이미지 경로 가져오기
    TArray<FString> ImagePaths = WBP_Image_Temp->GetImageList();

    // 멀티파트 폼 데이터 생성
    TArray<uint8> FormData = CreateMultipartFormData(ImagePaths, JsonString);

    // HTTP 요청 전송
    SendHttpRequest(FormData);
}

void UWBP_CreateWidget::CancleThisUI()
{
    RemoveFromParent();
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        PC->SetInputMode(FInputModeGameAndUI());
        PC->bShowMouseCursor = true;
    }
}

TArray<uint8> UWBP_CreateWidget::CreateMultipartFormData(const TArray<FString>& ImagePaths, const FString& JsonString) const
{
    TArray<uint8> Content;
    FString BeginBoundary = TEXT("--") + BOUNDARY + TEXT("\r\n");
    FString EndBoundary = TEXT("\r\n--") + BOUNDARY + TEXT("--\r\n");

    // JSON 데이터 파트 추가
    FString JsonPartHeader = BeginBoundary +
        TEXT("Content-Disposition: form-data; name=\"data\"\r\n\r\n") +
        JsonString + TEXT("\r\n");
        
    FTCHARToUTF8 JsonPartHeaderUtf8(*JsonPartHeader);
    Content.Append((uint8*)JsonPartHeaderUtf8.Get(), JsonPartHeaderUtf8.Length());

    // 이미지 파일들 추가
    for (int32 i = 0; i < ImagePaths.Num(); i++)
    {
        const FString& ImagePath = ImagePaths[i];
        FString FileName = FPaths::GetCleanFilename(ImagePath);

        // 이미지 파트 헤더
        FString FilePartHeader = BeginBoundary +
            TEXT("Content-Disposition: form-data; name=\"images\"; filename=\"") + FileName + TEXT("\"\r\n") +
            TEXT("Content-Type: image/png\r\n\r\n");

        FTCHARToUTF8 FilePartHeaderUtf8(*FilePartHeader);
        Content.Append((uint8*)FilePartHeaderUtf8.Get(), FilePartHeaderUtf8.Length());

        // 이미지 파일 데이터 추가
        TArray<uint8> FileData;
        if (FFileHelper::LoadFileToArray(FileData, *ImagePath))
        {
            Content.Append(FileData);
        }
    }

    // 종료 바운더리 추가
    FTCHARToUTF8 EndBoundaryUtf8(*EndBoundary);
    Content.Append((uint8*)EndBoundaryUtf8.Get(), EndBoundaryUtf8.Length());

    return Content;
}

void UWBP_CreateWidget::SendHttpRequest(const TArray<uint8>& FormData)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("http://www.mtvs.store:1818/api/v1/map/upload"));
    Request->SetVerb(TEXT("POST"));
    UAuthSubsystem* AuthSubsystem = GetGameInstance()->GetSubsystem<UAuthSubsystem>();
    FString Token = AuthSubsystem->GetAuthToken();
    Request->SetHeader("Authorization", FString::Printf(TEXT("Bearer %s"), *Token));
    Request->SetHeader(TEXT("Content-Type"), FString::Printf(TEXT("multipart/form-data; boundary=%s"), *BOUNDARY));
    Request->SetContent(FormData);
    Request->OnProcessRequestComplete().BindUObject(this, &UWBP_CreateWidget::OnHttpRequestComplete);
    Request->ProcessRequest();
}

void UWBP_CreateWidget::OnHttpRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded)
{
    if (bSucceeded && Response.IsValid())
    {
        int32 ResponseCode = Response->GetResponseCode();
        FString ResponseString = Response->GetContentAsString();

        if (ResponseCode == 200)
        {
            UE_LOG(LogTemp, Log, TEXT("Map upload successful: %s"), *ResponseString);

            if (SaveMapDataToFile(ResponseString))
            {
                // 레벨 인스턴스 정리
                if (APSH_PlayerController* PC = Cast<APSH_PlayerController>(GetWorld()->GetFirstPlayerController()))
                {
                    PC->ServerRequestCleanupCurrentRoom();
                }

                RemoveFromParent();
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to save map data"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Map upload failed with code %d: %s"), ResponseCode, *ResponseString);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HTTP Request failed"));
    }
}

bool UWBP_CreateWidget::SaveMapDataToFile(const FString& JsonString)
{
    // JSON 문자열을 파싱
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response"));
        return false;
    }

    // mapName과 dataTable 필드 확인
    if (!JsonObject->HasField(TEXT("mapName")) || !JsonObject->HasField(TEXT("dataTable")))
    {
        UE_LOG(LogTemp, Error, TEXT("JSON missing required fields"));
        return false;
    }

    // dataTable만 포함하는 새로운 JSON 문자열 생성
    FString SaveJsonString;
    TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> Writer =
        TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&SaveJsonString);

    // dataTable 배열을 직접 직렬화
    FJsonSerializer::Serialize(JsonObject->GetArrayField(TEXT("dataTable")), Writer);

    // 파일 저장 경로 설정
    FString MapName = JsonObject->GetStringField(TEXT("mapName"));
    FString SavePath = FPaths::ProjectSavedDir() / TEXT("MapData");

    // MapData 디렉토리 생성 확인
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*SavePath))
    {
        if (!PlatformFile.CreateDirectoryTree(*SavePath))
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create directory: %s"), *SavePath);
            return false;
        }
    }

    FString FullPath = SavePath / (MapName + TEXT(".json"));

    // 파일 저장
    if (!FFileHelper::SaveStringToFile(SaveJsonString, *FullPath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save map data to: %s"), *FullPath);
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("Successfully saved map data to: %s"), *FullPath);
    return true;
}

FString UWBP_CreateWidget::CreateBaseJson() const
{
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

    // 고정값 설정
    JsonObject->SetNumberField(TEXT("price"), 0);
    JsonObject->SetNumberField(TEXT("num_of_player"), 4);

    // UI에서 가져온 값들 설정
    JsonObject->SetStringField(TEXT("summary"), WBP_Sum_Title->GetSumTitle().ToString());
    JsonObject->SetStringField(TEXT("content"), WBP_Create_Level_Main_Input->GetMainText().ToString());
    JsonObject->SetStringField(TEXT("map_name"), WBP_Title_Box->GetTitle().ToString());
    JsonObject->SetStringField(TEXT("editable"), TEXT("ABLE"));

    // tags 배열 설정
    TArray<TSharedPtr<FJsonValue>> TagsArray;
    TagsArray.Add(MakeShareable(new FJsonValueString(TEXT("art"))));
    JsonObject->SetArrayField(TEXT("tags"), TagsArray);

    // data_table 필드에 데이터테이블 JSON 추가
    JsonObject->SetArrayField(TEXT("data_table"), GetDataTableJson());

    // JSON 문자열로 변환
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    return OutputString;
}

TArray<TSharedPtr<FJsonValue>> UWBP_CreateWidget::GetDataTableJson() const
{
    TArray<TSharedPtr<FJsonValue>> DataTableArray;

    if (!DT_PSH_SaveData)
    {
        UE_LOG(LogTemp, Warning, TEXT("DT_PSH_SaveData is null"));
        return DataTableArray;
    }

    TArray<FPSH_ObjectData*> DataArray;
    DT_PSH_SaveData->GetAllRows<FPSH_ObjectData>(TEXT(""), DataArray);

    for (const FPSH_ObjectData* Data : DataArray)
    {
        TSharedPtr<FJsonObject> RowObject = MakeShareable(new FJsonObject);
        FJsonObjectConverter::UStructToJsonObject(FPSH_ObjectData::StaticStruct(), Data, RowObject.ToSharedRef(), 0, 0);
        DataTableArray.Add(MakeShareable(new FJsonValueObject(RowObject)));
    }

    return DataTableArray;
}

