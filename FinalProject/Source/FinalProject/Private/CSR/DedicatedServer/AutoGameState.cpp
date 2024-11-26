// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/DedicatedServer/AutoGameState.h"
#include "Net/UnrealNetwork.h"
#include "CSR/DedicatedServer/AutoRoomManager.h"

void AAutoGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// AutoRoomManager 복제 설정을 Reliable로 설정
	DOREPLIFETIME_CONDITION_NOTIFY(AAutoGameState, AutoRoomManager, COND_None, REPNOTIFY_Always);
}

