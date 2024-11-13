// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PSH_PlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UPSH_PlayerAnim : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float direction;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
	class UAnimMontage * pick;
	UPROPERTY(EditDefaultsOnly, Category = Montage)
	class UAnimMontage * drop;

	UFUNCTION()
	void PlayAnimPickUp();

	UFUNCTION()
	void PlayAnimDrop();

};
