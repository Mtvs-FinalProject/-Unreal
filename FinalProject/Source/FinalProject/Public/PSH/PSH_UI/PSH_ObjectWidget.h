// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PSH_ObjectWidget.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UPSH_ObjectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox * Scroll_NomarlBlcok;

	UPROPERTY(meta = (BindWidget))
	class UButton * Btr_Back;

	UPROPERTY(EditDefaultsOnly , Category = "Data")
	class UDataTable * ObjectData;

	UPROPERTY(EditDefaultsOnly , Category = "Data")
	TSubclassOf<class UPSH_BlockWidget> blockWidgetFac;

	void AddchildBlock();

	UFUNCTION()
	void OnClickedBack();
};
