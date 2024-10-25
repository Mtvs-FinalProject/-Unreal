// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyFunctionMainUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class UMyFunctionMainUIWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	class UButton* Btn_Edit;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	class UButton* Btn_Play;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	class UButton* Btn_Close;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	bool bIsEditable;

	UFUNCTION()
	void OnEditButtonClicked();

	UFUNCTION()
	void OnPlayButtonClicked();

	UFUNCTION()
	void OnCloseButtonClcked();
};
