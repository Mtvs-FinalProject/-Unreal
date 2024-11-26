// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "MainPageWidget.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UMainPageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    
    // UI 컴포넌트 바인딩
    UPROPERTY(meta = (BindWidget))
    UButton * ServerListBTN;

    UPROPERTY(meta = (BindWidget))
    UButton * ShopBTN;

    UPROPERTY(meta = (BindWidget))
    UWidgetSwitcher * SwitchWidget;

protected:
    UFUNCTION()
    void OnClickServerListBTN();

    UFUNCTION()
    void OnClickShopBTN();
};
