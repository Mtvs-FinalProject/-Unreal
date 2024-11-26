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

	// ComboBox Widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UComboBoxString* FlyBoxList;

	//-----------------------------------------------------------------------------------------------
	// �պ� 
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCheckBox* Chk_LoopMode; // ���� �պ���� üũ �ڽ�

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCheckBox* Chk_SingleDirectionMode; // �ܼ� �̵� ��� üũ�ڽ�

	// �պ� Ƚ�� �Է� �ʵ�
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UEditableText* Txt_LoopCount;
	//------------------------------------------------------------------------------------------------

	// ���� ���õ� ������Ʈ
	AActor* SelectedActor = nullptr;

	// ALLFunctionObject: ������ ��� ������Ʈ���� �����ϴ� �迭
	UPROPERTY()
	TArray<AActor*> AllFunctionObject;
	
	UFUNCTION(BlueprintCallable)
	void OnUpButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnDownButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnStartButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnStopButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnOriginButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnBackButtonClicked();

	UFUNCTION(BlueprintCallable)
	void UpdateMovementValuesInUI(float SpeedValue, float DistanceValue);

	UFUNCTION(BlueprintCallable)
	void ApplyMovementValues();

	UFUNCTION(BlueprintCallable)
	void OnFlySpeedTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION(BlueprintCallable)
	void OnFlyDistanceTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> ActionChoice;

	// ������Ʈ ���ʸ� �������� �Լ� ����
	AActor* GetOwnerFromComponent();

	// �̵� ������ ����
	FVector StoredFlyDirection;

	UFUNCTION(BlueprintCallable)
	void InitializeFunctionObjects();

	UFUNCTION(BlueprintCallable)
	void AddObjectToComboBox(AActor* NewObject);

	UFUNCTION(BlueprintCallable)
	void OnFunctionObjectSelected(FString SelectedItem, ESelectInfo::Type SelectionType);

	//------------------------------------------------------------------------------------
	// �պ� �Լ�
	UFUNCTION(BlueprintCallable)
	void OnLoopModeCheckChanged(bool bIsChecked); // �պ� ��� üũ�ڽ� ���� ����

	UFUNCTION(BlueprintCallable)
	void OnSingleDirectionCheckChanged(bool bIsChecked); // �ܼ� �̵� ��� üũ�ڽ� ���� ����

	UFUNCTION(BlueprintCallable)
    void OnLoopCountCommitted(const FText& Text, ETextCommit::Type CommitMethod);  // �պ� Ƚ�� �Է� �� ȣ��

	//--------------------------------------------------------------------------------------

	// ��� ���Ͽ� �߰�
	UPROPERTY()
	TArray<UMyFlyActorComponent*> ControlledFlyComponents; // ���� FlyComponent�� ������ �迭

	UFUNCTION(BlueprintCallable)
	void AddControlledFlyComponent(UMyFlyActorComponent* NewFlyComponent);

	//-------------------------------------------------------------------------------------------
	   // Preview Actor ���� ���� �� �Լ�
	AActor* PreviewActor = nullptr;           // ������ ������Ʈ
	void SpawnPreviewActor();                 // ������ ���� ���� �Լ�
	void UpdatePreviewLocation(FVector Direction, float Distance);  // ������ ��ġ ������Ʈ
	void UpdatePreviewMovement();
	void DestroyPreviewActor();               // ������ ���� ����
	

	FTimerHandle PreviewFlyTimer;  // ������ ���� �̵� Ÿ�̸�
	float StoredMoveSpeed;			// �̵� �ӵ�
	float StoredMoveDistance;		// �̵� �Ÿ�

	// ���� ��ȯ ���� (�� ���� ���� �߰� �ʿ�)
	bool bPreviewDirectionReversed = false;   // �̵� ���� ���� ����

	// BP_MoveandFly�� �ʱ� ��ġ ����
	FVector MoveAndFlyInitialLocation;

	
};
