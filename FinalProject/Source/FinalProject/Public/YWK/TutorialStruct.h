// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TutorialStruct.generated.h"

USTRUCT(BlueprintType)
struct FTutorialStruct : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
    int32 ID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
    FString Speaker;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
    FString Text;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
    int32 NextID;

    FTutorialStruct() {
        this->ID = 0;
        this->NextID = 0;
        this->Speaker.Empty();
        this->Text.Empty();
    }
};
