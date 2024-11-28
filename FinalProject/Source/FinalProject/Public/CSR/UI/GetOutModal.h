// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GetOutModal.generated.h"


class APSH_PlayerController;
/**
 * 
 */
UCLASS()
class FINALPROJECT_API UGetOutModal : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	UPROPERTY()
	APSH_PlayerController *Player;

	UFUNCTION()
	void SetPlayer(APSH_PlayerController* Player_) {
		this->Player = Player_;
	}

    UPROPERTY(meta = (BindWidget))
	class UButton * YesBTN;

	UPROPERTY(meta = (BindWidget))
	class UButton * NoBTN;
	
	UFUNCTION()
	void OnClickYesBTN();

	UFUNCTION()
	void OnClickNoBTN();
};
