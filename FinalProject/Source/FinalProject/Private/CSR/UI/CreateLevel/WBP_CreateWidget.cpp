// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/CreateLevel/WBP_CreateWidget.h"
#include "PSH/PSH_DataTable/PSH_MechDataTable.h"
#include "JsonObjectConverter.h"
#include "CSR/Auth/AuthSubsystem.h"

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
    Token = TEXT("eyJhbGciOiJIUzI1NiJ9.eyJzdWIiOiIxIiwiaWF0IjoxNzMyNjA4Mzc5LCJleHAiOjE3MzMyMTMxNzl9.Nu-F64GuhEeJQZgU3BMpqNjdmRdlbc5ssfS08hVfPVQ");
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

            // UI 닫기
            RemoveFromParent();
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

