// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SumTitle.h"
#include "TitleBox.h"
#include "ImageTemp.h"
#include "CSR_W_CreateLevel_MainInput.h"
#include "CreateChooseWidget.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "WBP_CreateWidget.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UWBP_CreateWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	USumTitle *WBP_Sum_Title;

	UPROPERTY(meta = (BindWidget))
	UTitleBox *WBP_Title_Box;

	UPROPERTY(meta = (BindWidget))
	UImageTemp *WBP_Image_Temp;

	UPROPERTY(meta = (BindWidget))
	UCSR_W_CreateLevel_MainInput *WBP_Create_Level_Main_Input;

	UPROPERTY(meta = (BindWidget))
	UCreateChooseWidget * WBP_Click_Zone;
	 
	// 데이터테이블 설정 함수
	void SetDataTable(UDataTable* InDataTable) { DT_PSH_SaveData = InDataTable; }

protected:
	virtual void NativeConstruct();

private:
	 // 데이터테이블 레퍼런스
    UPROPERTY()
    class UDataTable* DT_PSH_SaveData;

    // 멀티파트 폼 데이터 생성을 위한 바운더리 상수
    const FString BOUNDARY = TEXT("----WebKitFormBoundary7MA4YWxkTrZu0gW");

    // 이벤트 핸들러
    UFUNCTION()
    void CreateMapHTTP();

    UFUNCTION()
    void CancleThisUI();

    // JSON 생성 함수
    FString CreateBaseJson() const;
    
    // 데이터테이블 JSON 변환 함수
    TArray<TSharedPtr<FJsonValue>> GetDataTableJson() const;

    // 멀티파트 폼 데이터 생성 함수
    TArray<uint8> CreateMultipartFormData(const TArray<FString>& ImagePaths, const FString& JsonString) const;

    // HTTP 요청 전송 함수
    void SendHttpRequest(const TArray<uint8>& FormData);

    // HTTP 응답 처리 함수
    void OnHttpRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded);

    bool SaveMapDataToFile(const FString& JsonString);
};
