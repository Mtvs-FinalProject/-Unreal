// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "SumTitle.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API USumTitle : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* SumTitle;

	FText GetSumTitle() const;
};
