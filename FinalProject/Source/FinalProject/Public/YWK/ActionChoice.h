// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ActionChoice.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UActionChoice : public UUserWidget
{
	GENERATED_BODY()
	

public:
	
UFUNCTION()
virtual void NativeConstruct() override;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_Move;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_Rotate;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_Fly;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* End_Button;

UFUNCTION()
void OnMoveClicked();

UFUNCTION()
void OnRotateClicked();

UFUNCTION()
void OnEndClicked();

UFUNCTION()
void OnFlyClicked();

// 처음화면으로 돌아가기
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
TSubclassOf<UUserWidget> FirstSelect;

// 회전으로 들어가기
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
TSubclassOf<UUserWidget> WBP_Rotate;

// 이동으로 들어가기
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
TSubclassOf<UUserWidget> WBP_Move;

// 날기로 들어가기
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
TSubclassOf<UUserWidget> WBP_Fly;

};
