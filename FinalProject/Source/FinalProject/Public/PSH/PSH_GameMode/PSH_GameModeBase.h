// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Delegates/Delegate.h"
#include "PSH_GameModeBase.generated.h"

 // ��������Ʈ ����
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
	// ������Ʈ ������ ȣ��� ��������Ʈ
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnObjectCreated OnObjectCreated;

	UPROPERTY(BlueprintAssignable, Category = "Block  Event")
	FOnStartBlock onStartBlock;

	// ������Ʈ ���� �Լ� ����
	UFUNCTION(BlueprintCallable, Category = "Object Management")
	AActor* SpawnFunctionObject();

	// �ٲٰ� ���� �� ���� ��.
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
