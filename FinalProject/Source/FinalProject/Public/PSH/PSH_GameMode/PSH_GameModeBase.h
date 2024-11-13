// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Delegates/Delegate.h"
#include "PSH_GameModeBase.generated.h"

/**
 * 
 */
 // 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectCreated, AActor*, NewObject);

UCLASS()
class FINALPROJECT_API APSH_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	// 오브젝트 생성시 호출될 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnObjectCreated OnObjectCreated;

	// 오브젝트 생성 함수 선언
	UFUNCTION(BlueprintCallable, Category = "Object Management")
	AActor* SpawnFunctionObject();


};
