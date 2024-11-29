// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/ServerListAndDetail/RoomDetailPanel.h"
#include "HttpModule.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "IImageWrapperModule.h"

void URoomDetailPanel::NativeConstruct()
{
    Super::NativeConstruct();
    DetailData.InitAllData();
    SetVisibility(ESlateVisibility::Hidden);
}

void URoomDetailPanel::RequestMapInfo(const FString& MapName)
{
    FString URL = FString::Printf(TEXT("http://www.mtvs.store:1818/api/v1/map/map-info/name/%s"), *MapName);

    UE_LOG(LogTemp, Warning, TEXT("csr %s"), *URL);
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetVerb("GET");
    HttpRequest->SetURL(URL);
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &URoomDetailPanel::OnHttpResponseReceived);
    HttpRequest->ProcessRequest();
}

void URoomDetailPanel::OnHttpResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
    if (!bSuccess || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("csr wwww"));

        return;
    }

    FString JsonString = Response->GetContentAsString();
    FMapDetailInfo MapInfo;

    if (ParseMapDetailFromJson(JsonString, MapInfo))
    {
        this->DetailData.Summary = MapInfo.Summary;
        this->DetailData.MaxPlayer = MapInfo.Player;
        this->DetailData.ImagesURL = MapInfo.ImagesURL;
        UpdateDetailInfo();
    }
}

void URoomDetailPanel::MakeDetailPanel(const FString& RoomName, const FString& Title, int32 CurrentPlayer)
{
    UE_LOG(LogTemp, Warning, TEXT("123 %s ,%s , %d"), *RoomName, *Title, CurrentPlayer);
	/*  if (Title.IsEmpty()) {
		  SetVisibility(ESlateVisibility::Hidden);
		  return ;
	  }*/
    SetVisibility(ESlateVisibility::Visible);
    this->DetailData.RoomName = RoomName;
    this->DetailData.MapName = Title;
    this->DetailData.CurrentPlayer = CurrentPlayer;
    RequestMapInfo(Title);
}

bool URoomDetailPanel::ParseMapDetailFromJson(const FString& JsonString, FMapDetailInfo& OutMapInfo)
{
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (!FJsonSerializer::Deserialize(Reader, JsonObject))
    {
        return false;
    }

    OutMapInfo.No = JsonObject->GetIntegerField(TEXT("no"));
    OutMapInfo.MapName = JsonObject->GetStringField(TEXT("mapName"));
    OutMapInfo.Summary = JsonObject->GetStringField(TEXT("summary"));
    OutMapInfo.Content = JsonObject->GetStringField(TEXT("content"));
    OutMapInfo.Price = JsonObject->GetIntegerField(TEXT("price"));
    OutMapInfo.Player = JsonObject->GetIntegerField(TEXT("player"));
    OutMapInfo.CreateAt = JsonObject->GetStringField(TEXT("createAt"));
    OutMapInfo.Editable = JsonObject->GetStringField(TEXT("editable"));

    // 배열 처리
    TArray<TSharedPtr<FJsonValue>> ImagesArray = JsonObject->GetArrayField(TEXT("imagesURL"));
    for (auto& ImageValue : ImagesArray)
    {
        OutMapInfo.ImagesURL.Add(ImageValue->AsString());
    }

    TArray<TSharedPtr<FJsonValue>> TagsArray = JsonObject->GetArrayField(TEXT("tags"));
    for (auto& TagValue : TagsArray)
    {
        OutMapInfo.Tags.Add(TagValue->AsString());
    }

    return true;
}

void URoomDetailPanel::LoadImageFromURL(const FString& ImageURL)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetVerb("GET");
    HttpRequest->SetURL(ImageURL);
    HttpRequest->OnProcessRequestComplete().BindLambda(
        [this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
        {
            if (bSuccess && Response.IsValid())
            {
                OnImageDownloaded(Response->GetContent());
            }
        });
    HttpRequest->ProcessRequest();
}

void URoomDetailPanel::OnImageDownloaded(TArray<uint8> ImageData)
{
    if (ImageData.Num() > 0)
    {
        if (PreviewImage)
        {
            // 이미지 데이터로부터 텍스처 생성
            UTexture2D* NewTexture = CreateTexture(ImageData);
            if (NewTexture)
            {
                // 동적 브러시 생성
                FSlateBrush Brush;
                Brush.SetResourceObject(NewTexture);

                // 이미지 위젯에 브러시 설정
                PreviewImage->SetBrush(Brush);
            }
        }
    }
}

UTexture2D* URoomDetailPanel::CreateTexture(const TArray<uint8>& PixelData)
{
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

    if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(PixelData.GetData(), PixelData.Num()))
    {
        TArray<uint8> UncompressedRGBA;
        if (ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, UncompressedRGBA))
        {
            UTexture2D* NewTexture = UTexture2D::CreateTransient(
                ImageWrapper->GetWidth(),
                ImageWrapper->GetHeight(),
                PF_R8G8B8A8
            );

            if (!NewTexture)
            {
                return nullptr;
            }

            // 텍스처 설정
            void* TextureData = NewTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
            FMemory::Memcpy(TextureData, UncompressedRGBA.GetData(), UncompressedRGBA.Num());
            NewTexture->GetPlatformData()->Mips[0].BulkData.Unlock();

            NewTexture->UpdateResource();
            return NewTexture;
        }
    }

    return nullptr;
}

void URoomDetailPanel::UpdateDetailInfo()
{
    if (TitleName)
    {
        TitleName->SetText(FText::FromString(DetailData.MapName));
    }

    if (Summary)
    {
        Summary->SetText(FText::FromString(DetailData.Summary));
    }

	if (RoomNamee)
    {
		RoomNamee->SetText(FText::FromString(DetailData.RoomName));
    }

    if (UserNum)
    {
        UserNum->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), DetailData.CurrentPlayer, DetailData.MaxPlayer)));
    }

    // 이미지 로딩 (첫 번째 이미지만)
    if (PreviewImage && DetailData.ImagesURL.Num() > 0)
    {
        LoadImageFromURL(DetailData.ImagesURL[0]);
    }
}
