// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Delegates/Delegate.h"
#include "PSH_GameModeBase.generated.h"

/**
 * 
 */
 // ��������Ʈ ����
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectCreated, AActor*, NewObject);

UCLASS()
class FINALPROJECT_API APSH_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	// ������Ʈ ������ ȣ��� ��������Ʈ
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnObjectCreated OnObjectCreated;

	// ������Ʈ ���� �Լ� ����
	UFUNCTION(BlueprintCallable, Category = "Object Management")
	AActor* SpawnFunctionObject();


};
