// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/DelegateCombinations.h"
#include "Components/Button.h"
#include "CreateChooseWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSaveButtonClickedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCancleButtonClickedDelegate);

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UCreateChooseWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UButton *SaveButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CancleButton;

	UPROPERTY(BlueprintAssignable, Category = "Button Events")
	FSaveButtonClickedDelegate SaveButtonClick;

	UPROPERTY(BlueprintAssignable, Category = "Button Events")
	FCancleButtonClickedDelegate CancleButtonClick;

protected:
	virtual void NativeConstruct() override;

    // 버튼 클릭 핸들러
    UFUNCTION()
    void HandleSaveButtonClicked();

    UFUNCTION()
	void HandleCancleButtonClicked();
};
