// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AutoGameState.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API AAutoGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
   UPROPERTY(Replicated)
   class AAutoRoomManager* AutoRoomManager;

   virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;		
};
