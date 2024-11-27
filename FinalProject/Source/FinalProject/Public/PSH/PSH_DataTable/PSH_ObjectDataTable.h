// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PSH_ObjectDataTable.generated.h"


USTRUCT(Atomic, BlueprintType)
struct FPSH_UiBlockData : public FTableRowBase// �ڽ� ������ ���̽� ������ ����.
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
 	class UTexture2D* icon; // ���� ���� ������

	UPROPERTY(EditAnywhere)
	TSubclassOf<class APSH_BlockActor> actor; // ���� ����

// 	UPROPERTY(EditAnywhere)
// 	class UStaticMesh * mesh; // û����(������)���� ���� �޽� ������
	FPSH_UiBlockData() {
		this->icon = nullptr;
	}
};

UCLASS()
class FINALPROJECT_API UPSH_ObjectDataTable : public UDataTable
{
	GENERATED_BODY()
	
};


