// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/PSH_Component/PSH_PlayerAnim.h"
#include "PSH/PSH_Player/PSH_Player.h"

void UPSH_PlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APSH_Player* player = Cast<APSH_Player>(TryGetPawnOwner());
	if (nullptr == player) return;

	FVector velocity = player->GetVelocity();
	FVector forwardVector = player->GetActorForwardVector();
	FVector rightVector = player->GetActorRightVector();

	// 菊 第 加档
	Speed = FVector::DotProduct(forwardVector, velocity);

	// 谅快 加档
	direction = FVector::DotProduct(rightVector, velocity);

}
void UPSH_PlayerAnim::PlayAnimPickUp()
{
	Montage_Play(pick);
}
void UPSH_PlayerAnim::PlayAnimDrop()
{
	Montage_Play(drop);
}