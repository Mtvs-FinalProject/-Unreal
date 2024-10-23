// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PSH_BlockWidget.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UPSH_BlockWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UButton * Btn_Icon;

	UFUNCTION()
	void SpawnBlock();

	UPROPERTY()
	TSubclassOf<class APSH_BlockActor> spawnActor;

};
