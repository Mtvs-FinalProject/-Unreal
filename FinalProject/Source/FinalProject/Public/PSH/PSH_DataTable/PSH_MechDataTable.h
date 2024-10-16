// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PSH_MechDataTable.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FPSH_ObjectData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

// 	UPROPERTY(EditAnywhere)
// 	class UTexture2D* icon;
// 
// 	class UStaticMesh * mesh;

	UPROPERTY(EditAnywhere)
	class APSH_BlockActor * actor;
};

UCLASS()
class FINALPROJECT_API UPSH_MechDataTable : public UDataTable
{
	GENERATED_BODY()
	
};
