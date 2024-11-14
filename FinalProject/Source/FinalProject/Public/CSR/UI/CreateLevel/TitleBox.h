// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "TitleBox.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UTitleBox : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Title;

	FText GetTitle() const;
};
