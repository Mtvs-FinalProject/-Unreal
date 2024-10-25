// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PSH_GarbageBotWidget.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UPSH_GarbageBotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btr_Move;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btr_Destroy;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btr_Idle;


	UFUNCTION()
	void OnClickMove();
	UFUNCTION()
	void OnClickDestroy();
	UFUNCTION()
	void OnClickIdle();

	void SetOwner(class APSH_GarbageBot * owner);

	class APSH_GarbageBot * bot;
};
