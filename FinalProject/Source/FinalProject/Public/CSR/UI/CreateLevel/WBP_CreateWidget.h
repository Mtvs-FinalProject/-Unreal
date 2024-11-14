// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SumTitle.h"
#include "TitleBox.h"
#include "ImageTemp.h"
#include "CSR_W_CreateLevel_MainInput.h"
#include "CreateChooseWidget.h"
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
	
	void RP_Report(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

protected:
	virtual void NativeConstruct();

private:
	UFUNCTION()
	void CreateMapHTTP();

	UFUNCTION()
	void CancleThisUI();

	UFUNCTION()
	FString MakeJson(const FString& Title, const FString& Sub_Title, const FString& Main_List, const TArray<FString>& ImagePaths) const;
};
