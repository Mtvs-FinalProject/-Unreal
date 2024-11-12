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
	class UButton * Btr_NormalLeft;
	UPROPERTY(meta = (BindWidget))
	class UButton * Btr_NormalRight;

	UPROPERTY(meta = (BindWidget))
	class UButton * Btr_FunctionLeft;
	UPROPERTY(meta = (BindWidget))
	class UButton * Btr_FunctionRight;

	UPROPERTY(meta = (BindWidget))
	class UButton * Btr_Back;
	UPROPERTY(meta = (BindWidget))
	class UButton * Btr_Save;
	UPROPERTY(meta = (BindWidget))
	class UButton * Btr_Load;
// 	UPROPERTY(meta = (BindWidget))
// 	class UButton * Btr_CallBot;

	UPROPERTY(EditDefaultsOnly , Category = "Data")
	class UDataTable * nomalObjectData;
	UPROPERTY(EditDefaultsOnly , Category = "Data")
	class UDataTable * functionObjectData;

	UPROPERTY(EditDefaultsOnly , Category = "Data")
	TSubclassOf<class UPSH_BlockWidget> blockWidgetFac;

	void AddchildBlock();

	void AddNomalBlock();

	void AddFunctionBlock();

	UFUNCTION()
	void OnClickedBack();

	UFUNCTION()
	void OnClickedSave();

	UFUNCTION()
	void OnClickedLoad();

// 	UFUNCTION()
// 	void OnClickedCallBot();

	
	int32 CurrentIndex = 0;

	float ScrollBoxHeight;

	float nomalScrollOffset = 0.0f;
	bool bIsNomalScrolling = false;

	float functionScrollOffset = 0.0f;
	bool bIsFunctionScrolling = false;

	UFUNCTION()
	void OnNormalScrollRightClicked();

	UFUNCTION()
	void OnNormalScrollLeftClicked();

	UFUNCTION()
	void OnFunctionScrollLeftClicked();

	UFUNCTION()
	void OnFunctionScrollRightClicked();
};
