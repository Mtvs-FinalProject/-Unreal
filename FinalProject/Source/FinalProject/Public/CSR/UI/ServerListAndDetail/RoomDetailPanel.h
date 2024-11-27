// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Delegates/DelegateCombinations.h"
#include "RoomDetailPanel.generated.h"

USTRUCT(BlueprintType)
struct FMapDetailInfo
{
    GENERATED_BODY()

    UPROPERTY()
    int32 No;

    UPROPERTY()
    FString MapName;

    UPROPERTY()
    FString Summary;

    UPROPERTY()
    FString Content;

    UPROPERTY()
    int32 Price;

    UPROPERTY()
    TArray<FString> ImagesURL;

    UPROPERTY()
    TArray<FString> Tags;

    UPROPERTY()
    FString Editable;

    UPROPERTY()
    int32 Player;

    UPROPERTY()
    FString CreateAt;

    FMapDetailInfo() {
        this->No = 0;
        this->MapName.Empty();
        this->Summary.Empty();
        this->Content.Empty();
        this->Price = 0;
        this->ImagesURL.Empty();
        this->Tags.Empty();
        this->Editable.Empty();
        this->Player = 0;
        this->CreateAt.Empty();
    }

    void InitAllData() {
        this->No = 0;
        this->MapName.Empty();
        this->Summary.Empty();
        this->Content.Empty();
        this->Price = 0;
        this->ImagesURL.Empty();
        this->Tags.Empty();
        this->Editable.Empty();
        this->Player = 0;
        this->CreateAt.Empty();
    }
};

USTRUCT(BlueprintType)
struct FRoomDetailPanelInfo
{
    GENERATED_BODY()

    UPROPERTY()
    FString MapName;

    UPROPERTY()
    FString Summary;

    UPROPERTY()
    TArray<FString> ImagesURL;

    UPROPERTY()
    FString RoomName;

    UPROPERTY()
    int32 CurrentPlayer;

    UPROPERTY()
    int32 MaxPlayer;

    FRoomDetailPanelInfo() {
        this->MapName.Empty();
        this->Summary.Empty();
        this->ImagesURL.Empty();
        this->RoomName.Empty();
        this->CurrentPlayer = 0;
        this->MaxPlayer = 0;
    }

    void InitAllData() {
        this->MapName.Empty();
        this->Summary.Empty();
        this->ImagesURL.Empty();
        this->RoomName.Empty();
        this->CurrentPlayer = 0;
        this->MaxPlayer = 0;
    }
};

/**
 * 
 */
UCLASS()
class FINALPROJECT_API URoomDetailPanel : public UUserWidget
{
	GENERATED_BODY()

public:
    // Detail UI 업데이트
    void UpdateDetailInfo();
    
    // HTTP 요청 시작
    void RequestMapInfo(const FString& RoomName);

    UFUNCTION(BlueprintCallable)
    void MakeDetailPanel(const FString & RoomName, const FString & Title, int32 CurrentPlayer);

protected:
    virtual void NativeConstruct() override;

    // HTTP 응답 처리
    void OnHttpResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);

    // UI 엘리먼트들
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* RoomNamee;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TitleName;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Summary;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* UserNum;

    // 이미지 표시용
    UPROPERTY(meta = (BindWidget))
    class UImage* PreviewImage;

    UPROPERTY()
    FRoomDetailPanelInfo DetailData;

private:
    // JSON 파싱
    bool ParseMapDetailFromJson(const FString& JsonString, FMapDetailInfo& OutMapInfo);	

    // 이미지 다운로드 및 표시
    void LoadImageFromURL(const FString& ImageURL);

    // 텍스처 로드 완료 시 호출될 함수
    void OnImageDownloaded(TArray<uint8> ImageData);

    // 다운로드한 이미지 데이터를 텍스처로 변환
    UTexture2D* CreateTexture(const TArray<uint8>& PixelData);
};
