// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PSH_ObjectWidget.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UPSH_ObjectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox * Scroll_NomarlBlcok;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox * Scroll_FunctionBlcok;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox * Scroll_MapBlcok;

	UPROPERTY(meta = (BindWidget))
	class UButton * Btr_NormalLeft;
	UPROPERTY(meta = (BindWidget))
	class UButton * Btr_NormalRight;

	UPROPERTY(meta = (BindWidget))
	class UButton * Btr_FunctionLeft;
	UPROPERTY(meta = (BindWidget))
	class UButton * Btr_FunctionRight;

	UPROPERTY(meta = (BindWidget))
	class UButton * Btr_MapLeft;
	
	UPROPERTY(meta = (BindWidget))
	class UButton * Btr_MapRight;

	UPROPERTY(meta = (BindWidget))
	class UButton * Btr_Back;
	UPROPERTY(meta = (BindWidget))
	class UButton * Btr_Save;
	UPROPERTY(meta = (BindWidget))
	class UButton * Btr_Load;

	UPROPERTY(EditDefaultsOnly , Category = "Data")
	class UDataTable * nomalObjectData;

	UPROPERTY(EditDefaultsOnly , Category = "Data")
	class UDataTable * functionObjectData;
	UPROPERTY(EditDefaultsOnly , Category = "Data")
	class UDataTable * mapObjectData;

	UPROPERTY(EditDefaultsOnly , Category = "Data")
	TSubclassOf<class UPSH_BlockWidget> blockWidgetFac;

	UPROPERTY(meta = (BindWidgetAnim),Transient)
	class UWidgetAnimation * Spawn;
	UPROPERTY(meta = (BindWidgetAnim),Transient)
	class UWidgetAnimation * Close;

	FWidgetAnimationDynamicEvent endDelegate;

	UFUNCTION()
	void AnimEnd();

	UFUNCTION()
	void StartSpawnAnim();
	UFUNCTION() 
	void EndSpawnAnim();

	void AddchildBlock();

	void AddNomalBlock();

	void AddFunctionBlock();

	void AddMapBlock();

	UFUNCTION()
	void OnClickedBack();

	UFUNCTION()
	void OnClickedSave();

	UFUNCTION()
	void OnClickedLoad();

// 	UFUNCTION()
// 	void OnClickedCallBot();

	
	int32 CurrentIndex = 0;

	float nomalScrollOffset = 0.0f;
	bool bIsNomalScrolling = false;

	float functionScrollOffset = 0.0f;
	bool bIsFunctionScrolling = false;

	float mapScrollOffset = 0.0f;
	bool bIsmapScrolling = false;

	UFUNCTION()
	void OnNormalScrollRightClicked();

	UFUNCTION()
	void OnNormalScrollLeftClicked();

	UFUNCTION()
	void OnFunctionScrollLeftClicked();

	UFUNCTION()
	void OnFunctionScrollRightClicked();

	UFUNCTION()
	void OnMapScrollRightClicked();

	UFUNCTION()
	void OnMapScrollLeftClicked();
};
