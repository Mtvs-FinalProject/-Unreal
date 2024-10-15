// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "PSH_SnapPoint.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UPSH_SnapPoint : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:

/*	virtual void ev*/

	virtual void BeginPlay() override; // 안불러진다..?

	void DataSyncComplete();

	UPROPERTY(EditAnywhere)
	int32 priority;
};
