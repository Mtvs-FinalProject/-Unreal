// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PSH_MechDataTable.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FPSH_ChildData // �ڽ� ������ ���̽� ������ ����.
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<class APSH_BlockActor> actor; // ���� ����

	UPROPERTY(EditAnywhere)
	FTransform actorTransfrom; // ���� ��ġ


};
USTRUCT(Atomic, BlueprintType)
struct  FPSH_Childdats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FPSH_ChildData> childData; // �ڽ� ���� ������
};

USTRUCT(Atomic, BlueprintType)
struct FPSH_ObjectData : public FTableRowBase // ������ ���̽� ������ ����.
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<class APSH_BlockActor> actor; // ���� ����

	UPROPERTY(EditAnywhere)
	FTransform actorTransfrom; // ���� ��ġ

	
	UPROPERTY(EditAnywhere)
	TArray<FPSH_Childdats> childsData; // �ڽ� ���� ������
	// ���� �̸�
};

UCLASS()
class FINALPROJECT_API UPSH_MechDataTable : public UDataTable
{
	GENERATED_BODY()
	
};
