// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Delegates/Delegate.h"
#include "PSH_GameModeBase.generated.h"

 // 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectCreated, AActor*, NewObject);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStartBlock, bool, createMode);

UENUM(BlueprintType)

enum class ECreateMode : uint8
{
	PLAY,
	CREATE
};

UCLASS()
class FINALPROJECT_API APSH_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	// 오브젝트 생성시 호출될 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnObjectCreated OnObjectCreated;

	UPROPERTY(BlueprintAssignable, Category = "Block  Event")
	FOnStartBlock onStartBlock;

	// 오브젝트 생성 함수 선언
	UFUNCTION(BlueprintCallable, Category = "Object Management")
	AActor* SpawnFunctionObject();

	// 바꾸고 싶은 것 적는 곳.
	void SetMode(ECreateMode changeMode);

	ECreateMode GetMode();

	bool GetCreateingCheck();

	void SetCreateingCheck(bool check);

	UFUNCTION(NetMulticast,Reliable)
	void MRPC_StartBlcok();


private:
	bool bCreateMode = true;
	ECreateMode mode;

};
