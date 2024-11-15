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

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
bool bIsEditMode;

UFUNCTION()
void OnActionClicked();

UFUNCTION()
void OnEndClicked();

UFUNCTION()
void OnCraftClicked();

UFUNCTION()
void OnDestroyClicked();

UFUNCTION()
void OnBtn_SaveClicked();

UFUNCTION(BlueprintImplementableEvent)
void MapSave();

// 행동으로 들어가기
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
TSubclassOf<UUserWidget> ActionChoice;

// 오브젝트 위젯으로 들어가기
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
TSubclassOf<UUserWidget> PSH_ObjectWidget;

};
