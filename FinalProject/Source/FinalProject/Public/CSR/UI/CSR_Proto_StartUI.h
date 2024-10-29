// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "CSR_Proto_StartUI.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UCSR_Proto_StartUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UButton* BTN_CreateRoom;

    UPROPERTY(meta = (BindWidget))
    UButton* BTN_LoadRoom;

    UPROPERTY(meta = (BindWidget))
    UButton* BTN_Exit;

    UFUNCTION(BlueprintImplementableEvent)
    void OnCreateRoomClicked();
    
    UFUNCTION(BlueprintImplementableEvent)
    void OnLoadRoomClicked();

    UFUNCTION(BlueprintImplementableEvent)
    void OnExitClicked();
};
