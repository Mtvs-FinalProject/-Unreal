// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FirstSelect.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UFirstSelect : public UUserWidget
{
	GENERATED_BODY()
	

public:

UFUNCTION()
virtual void NativeConstruct() override;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_Action;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_Save;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_Craft;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_Destroy;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_end;

UFUNCTION()
void OnActionClicked();

UFUNCTION()
void OnEndClicked();

UFUNCTION()
void OnCraftClicked();

UFUNCTION()
void OnDestroyClicked();

// 행동으로 들어가기
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
TSubclassOf<UUserWidget> ActionChoice;

};
