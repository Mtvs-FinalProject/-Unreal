// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "ShopMainWidget.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UShopMainWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton * MapDownBTN;

	UPROPERTY(meta = (BindWidget))
	UButton * AvatarBTN;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher * ShopSwitch;

	UFUNCTION()
	void OnClickMapDownBTN();

	UFUNCTION()
	void OnClickAvatarBTN();
};
