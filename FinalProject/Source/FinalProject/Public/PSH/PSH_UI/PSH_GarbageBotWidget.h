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

	UPROPERTY(meta = (BindWidget))
	class UButton* Btr_Bot;

	UPROPERTY(meta = (BindWidgetAnim),Transient)
	class UWidgetAnimation * ButtonHoverVisibleAnim;
	UPROPERTY(meta = (BindWidgetAnim),Transient)
	class UWidgetAnimation * ButtonHoverHiddenAnim;

	class APSH_Player * player;

	//Button
	UFUNCTION()
	void OnClickMove();
	UFUNCTION()
	void OnClickDestroy();
	UFUNCTION()
	void OnClickIdle();

	//BackGround
	UFUNCTION()
	void OnClickBackGround();

	FWidgetAnimationDynamicEvent endDelegate;

	// Bot
	UFUNCTION()
	void OnClickBot();
	UFUNCTION()
	void OnHoveredBot();


	bool bHoverdBot = false;

	void SetModeButtonUI(bool chek);

	class APSH_GarbageBot * bot;
};
