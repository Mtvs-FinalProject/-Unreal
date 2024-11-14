// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/CreateLevel/WBP_CreateWidget.h"
#include "Interfaces/IHttpRequest.h"
#include "HttpModule.h"

void UWBP_CreateWidget::NativeConstruct()
{
	this->WBP_Click_Zone->SaveButtonClick.AddDynamic(this, &UWBP_CreateWidget::CreateMapHTTP);
	this->WBP_Click_Zone->CancleButtonClick.AddDynamic(this, &UWBP_CreateWidget::CancleThisUI);
}

void UWBP_CreateWidget::CreateMapHTTP()
{
    FString Title = FString::Printf(TEXT("%s"), *this->WBP_Title_Box->GetTitle().ToString());
	FString Sub_Title = FString::Printf(TEXT("%s"), *this->WBP_Sum_Title->GetSumTitle().ToString());
	FString Main_List = FString::Printf(TEXT("%s"), *this->WBP_Create_Level_Main_Input->GetMainText().ToString());
    TArray<FString> ImagePaths = this->WBP_Image_Temp->GetImageList();
	FString JsonString = MakeJson(Title, Sub_Title, Main_List, ImagePaths);

    // HTTP 모듈과 요청 객체를 생성
    FHttpModule* Http = &FHttpModule::Get();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

    // 요청 URL, 메서드 및 헤더 설정
    Request->SetURL(TEXT("http://yourserver.com/api/upload"));  // 서버 URL을 여기에 설정
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("content-type"), TEXT("application/json"));

    // JSON 데이터를 본문에 설정
    Request->SetContentAsString(JsonString);

    // 요청 완료 콜백 설정
    Request->OnProcessRequestComplete().BindUObject(this, &UWBP_CreateWidget::RP_Report);

    // 요청 전송
    Request->ProcessRequest();
}

void UWBP_CreateWidget::CancleThisUI()
{
	this->SetVisibility(ESlateVisibility::Hidden);
}

FString UWBP_CreateWidget::MakeJson(const FString &Title, const FString &Sub_Title, const FString &Main_List, const TArray<FString>& ImagePaths) const
{
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    JsonObject->SetStringField("title", Title);
    JsonObject->SetStringField("sub", Sub_Title);
    JsonObject->SetStringField("main", Main_List);

    // Base64 이미지 배열 추가
    TArray<TSharedPtr<FJsonValue>> ImageArray;
    for (const FString &ImagePath : ImagePaths) // 전달된 ImagePaths 배열을 사용
    {
        TArray<uint8> ImageData;
        if (FFileHelper::LoadFileToArray(ImageData, *ImagePath))
        {
            FString Base64Image = FBase64::Encode(ImageData);
            ImageArray.Add(MakeShareable(new FJsonValueString(Base64Image)));
        }
    }
    JsonObject->SetArrayField("images", ImageArray);

    // JSON 객체를 JSON 문자열로 직렬화
    FString JsonString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    return JsonString;
}

void UWBP_CreateWidget::RP_Report(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("Request succeeded: %s"), *Response->GetContentAsString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Request failed"));
    }
}
