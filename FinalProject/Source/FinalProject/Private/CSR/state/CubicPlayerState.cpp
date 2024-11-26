// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/state/CubicPlayerState.h"
#include "Net/UnrealNetwork.h"

ACubicPlayerState::ACubicPlayerState()
{
	bReplicates = true;
	LinkedCharacter = nullptr;
}

void ACubicPlayerState::SetLinkedCharacter(APSH_Player* NewCharacter)
{
	LinkedCharacter = NewCharacter;
}

APSH_Player* ACubicPlayerState::GetLinkedCharacter() const
{
	return (LinkedCharacter);
}

void ACubicPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACubicPlayerState, LinkedCharacter);
}
