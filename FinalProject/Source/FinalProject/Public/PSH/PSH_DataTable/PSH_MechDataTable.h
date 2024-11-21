// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PSH_MechDataTable.generated.h"


USTRUCT(Atomic, BlueprintType)
struct FPSH_FunctionBlockData // 자식 데이터 베이스 저장할 공간.
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	TArray<int32> intArray; // 인트 변수 배열

	UPROPERTY(EditAnywhere)
	TArray<float> floatArray;  // 인트 변수 배열

	UPROPERTY(EditAnywhere)
	TArray<FVector> fvectorArray;  // 인트 변수 배열

	UPROPERTY(EditAnywhere)
	TArray<FRotator> frotatorArray;

	UPROPERTY(EditAnywhere)
	TArray<bool> boolArray;  // 인트 변수 배열

};

USTRUCT(Atomic, BlueprintType)
struct FPSH_ChildData // 자식 데이터 베이스 저장할 공간.
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FPSH_FunctionBlockData> funcitonData; // funciton 블럭에 저장된 변수들

	UPROPERTY(EditAnywhere)
	TSubclassOf<class APSH_BlockActor> actor; // 블럭의 외형

	UPROPERTY(EditAnywhere)
	FTransform actorTransfrom; // 블럭의 위치
};

USTRUCT(Atomic, BlueprintType)
struct  FPSH_Childdats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FPSH_ChildData> childData; // 자식 블럭의 정보들
};

USTRUCT(Atomic, BlueprintType)
struct FPSH_ObjectData : public FTableRowBase // 데이터 베이스 저장할 공간.
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FPSH_FunctionBlockData> funcitonData; // funciton 블럭에 저장된 변수들

	UPROPERTY(EditAnywhere)
	TSubclassOf<class APSH_BlockActor> actor; // 블럭의 외형

	UPROPERTY(EditAnywhere)
	FTransform actorTransfrom; // 블럭의 위치

	UPROPERTY(EditAnywhere)
	TArray<FPSH_Childdats> childsData; // 자식 블럭의 정보들
	// 개별 이름
};

UCLASS()
class FINALPROJECT_API UPSH_MechDataTable : public UDataTable
{
	GENERATED_BODY()
	
};
