// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Tutorial_Widget.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UTutorial_Widget : public UUserWidget
{
	GENERATED_BODY()

	void NativeConstruct() override;
	

public:
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void UpdateText(const FString& NewText);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Tutorial_Txt;

	UPROPERTY(meta = (BindWidget))
	class UImage* Tutorial_Img;

	UPROPERTY()
    class ATutorialBot* TutorialBot;
};
