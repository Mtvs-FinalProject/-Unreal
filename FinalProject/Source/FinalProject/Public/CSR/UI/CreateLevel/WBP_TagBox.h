// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "WBP_TagBox.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UWBP_TagBox : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UButton *AddTag;

protected:
	virtual void NativeConstruct() override;
};
