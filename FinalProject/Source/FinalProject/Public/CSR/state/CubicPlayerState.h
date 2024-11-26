// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PSH/PSH_Player/PSH_Player.h"
#include "CubicPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API ACubicPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ACubicPlayerState();

protected:
	// 플레이어의 캐릭터
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	APSH_Player* LinkedCharacter;
public:
	// 캐릭터 설정
	UFUNCTION(BlueprintCallable, Category = "Player")
	void SetLinkedCharacter(APSH_Player* NewCharacter);

	// 캐릭터 가져오기
	UFUNCTION(BlueprintCallable, Category = "Player")
	APSH_Player* GetLinkedCharacter() const;
protected:
	// 네트워크 복제를 위한 설정
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
