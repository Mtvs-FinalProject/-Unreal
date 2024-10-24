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
	
	// �̵����� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WBP_Move;

	// ����� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> WBP_Fly;

};
