// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FlyWidget.generated.h"

/**
 * 
 */
UCLASS()
class UFlyWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* Btn_UP;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* Btn_Down;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* Btn_Back;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* Btn_Start;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* Btn_Stop;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* Btn_Origin;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UEditableText* FlySpeedText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UEditableText* FlyHightText;
	
	UFUNCTION()
	void OnUpButtonClicked();

	UFUNCTION()
	void OnDownButtonClicked();

	UFUNCTION()
	void OnStartButtonClicked();

	UFUNCTION()
	void OnStopButtonClicked();

	UFUNCTION()
	void OnOriginButtonClicked();

	UFUNCTION()
	void OnBackButtonClicked();

	UFUNCTION()
	void UpdateMovementValuesInUI(float SpeedValue, float DistanceValue);

	UFUNCTION()
	void ApplyMovementValues();

	UFUNCTION()
	void OnFlySpeedTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnFlyDistanceTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> ActionChoice;

	// 컴포넌트 오너를 가져오는 함수 선언
	AActor* GetOwnerFromComponent();

	// 이동 저장할 변수
	FVector StoredFlyDirection;


	
};
