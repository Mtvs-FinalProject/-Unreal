// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyChoiceActionWidget.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UMyChoiceActionWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* Btn_Move;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* Btn_Fly;
	//----------------------------------------------------------------------

	UFUNCTION()
	void OnMoveButtonClicked();

	UFUNCTION()
	void OnFlyButtonClicked();
    //-----------------------------------------------------------------------
	
	// 이동으로 들어가기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WBP_Move;

	// 날기로 들어가기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WBP_Fly;

};
