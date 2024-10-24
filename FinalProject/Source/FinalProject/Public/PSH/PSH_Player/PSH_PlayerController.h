// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PSH_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API APSH_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APSH_PlayerController();

	virtual void PlayerTick(float DeltaTime) override;

	virtual void BeginPlay() override;

	void LookMouseCursor();

	bool targetLockOn = false;

	//Widgets 
	
};
