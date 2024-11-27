// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PSH_ObjectDataTable.generated.h"


USTRUCT(Atomic, BlueprintType)
struct FPSH_UiBlockData : public FTableRowBase// 자식 데이터 베이스 저장할 공간.
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
 	class UTexture2D* icon; // 툴에 사용될 아이콘

	UPROPERTY(EditAnywhere)
	TSubclassOf<class APSH_BlockActor> actor; // 블럭의 외형

// 	UPROPERTY(EditAnywhere)
// 	class UStaticMesh * mesh; // 청사진(프리뷰)에서 사용될 메쉬 데이터
	FPSH_UiBlockData() {
		this->icon = nullptr;
	}
};

UCLASS()
class FINALPROJECT_API UPSH_ObjectDataTable : public UDataTable
{
	GENERATED_BODY()
	
};


