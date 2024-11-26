// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PSH_MechDataTable.generated.h"


USTRUCT(Atomic, BlueprintType)
struct FPSH_FunctionBlockData // �ڽ� ������ ���̽� ������ ����.
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	TArray<int32> intArray; // ��Ʈ ���� �迭

	UPROPERTY(EditAnywhere)
	TArray<float> floatArray;  // ��Ʈ ���� �迭

	UPROPERTY(EditAnywhere)
	TArray<FVector> fvectorArray;  // ��Ʈ ���� �迭

	UPROPERTY(EditAnywhere)
	TArray<FRotator> frotatorArray;

	UPROPERTY(EditAnywhere)
	TArray<bool> boolArray;  // ��Ʈ ���� �迭

};

USTRUCT(Atomic, BlueprintType)
struct FPSH_BlockData // �ڽ� ������ ���̽� ������ ����.
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FPSH_FunctionBlockData> funcitonData; // funciton ���� ����� ������

	UPROPERTY(EditAnywhere)
	TSubclassOf<class APSH_BlockActor> actor; // ���� ����

	UPROPERTY(EditAnywhere)
	FTransform actorTransform; // ���� ��ġ
	
	TArray<FPSH_BlockData> childData;

};

USTRUCT(Atomic, BlueprintType)
struct FPSH_ObjectData : public FTableRowBase // ������ ���̽� ������ ����.
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FPSH_BlockData blockData;
	
	UPROPERTY(EditAnywhere)
	bool bisSave = false;
};

UCLASS()
class FINALPROJECT_API UPSH_MechDataTable : public UDataTable
{
	GENERATED_BODY()
	
};
