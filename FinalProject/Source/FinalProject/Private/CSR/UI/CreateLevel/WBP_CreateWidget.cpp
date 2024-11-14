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

    // HTTP ���� ��û ��ü�� ����
    FHttpModule* Http = &FHttpModule::Get();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

    // ��û URL, �޼��� �� ��� ����
    Request->SetURL(TEXT("http://yourserver.com/api/upload"));  // ���� URL�� ���⿡ ����
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("content-type"), TEXT("application/json"));

    // JSON �����͸� ������ ����
    Request->SetContentAsString(JsonString);

    // ��û �Ϸ� �ݹ� ����
    Request->OnProcessRequestComplete().BindUObject(this, &UWBP_CreateWidget::RP_Report);

    // ��û ����
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

    // Base64 �̹��� �迭 �߰�
    TArray<TSharedPtr<FJsonValue>> ImageArray;
    for (const FString &ImagePath : ImagePaths) // ���޵� ImagePaths �迭�� ���
    {
        TArray<uint8> ImageData;
        if (FFileHelper::LoadFileToArray(ImageData, *ImagePath))
        {
            FString Base64Image = FBase64::Encode(ImageData);
            ImageArray.Add(MakeShareable(new FJsonValueString(Base64Image)));
        }
    }
    JsonObject->SetArrayField("images", ImageArray);

    // JSON ��ü�� JSON ���ڿ��� ����ȭ
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
