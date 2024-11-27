// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyMoveActorComponent.h"
#include "Movewidget.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UMovewidget : public UUserWidget
{
	GENERATED_BODY()

public:

UFUNCTION()
virtual void NativeConstruct() override;

//-------------------------------------------------------------------------------------------------------------------------------------------
UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_Front;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_Back;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_Right;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_Left;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_MoveBack;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_MoveStart;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_MoveStop;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_MoveOrigin;

// ALLFunctionObject: ������ ��� ������Ʈ���� �����ϴ� �迭
UPROPERTY()
TArray<AActor*> AllFunctionObject;

// ComboBox Widget
UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
class UComboBoxString* MoveBoxList;

// ���� ���õ� ������Ʈ
AActor* SelectedActor = nullptr;

//------------------------------------------------------------------------------------------------------------------------------------------
UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UEditableText* SpeedMoveText;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UEditableText* DistMoveText;
//------------------------------------------------------------------------------------------------------------------------------------------

// �պ� ���� Ƚ�� �Է� ����
UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UCheckBox* Chk_LoopMode;  // ���� �պ� ��� üũ�ڽ�

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UCheckBox* Chk_SingleDirectionMode;  // �ܼ� �̵� ��� üũ�ڽ�
//------------------------------------------------------------------------------------------------------------------------------------------
UFUNCTION(BlueprintCallable)
void OnFrontClicked();

UFUNCTION(BlueprintCallable)
void OnBackClicked();

UFUNCTION(BlueprintCallable)
void OnRightClicked();

UFUNCTION(BlueprintCallable)
void OnLeftClicked();

UFUNCTION(BlueprintCallable)
void OnMoveBackClicked();

UFUNCTION(BlueprintCallable)
void OnStartButtonClicked();

UFUNCTION(BlueprintCallable)
void OnStopButtonClicked();

UFUNCTION(BlueprintCallable)
void OnOriginButtonClicked();

//�ӵ��� SetText�� �����ϱ�
UFUNCTION(BlueprintCallable)
void UpdateMovementValuesInUI(float SpeedValue, float DistanceValue);

// �ӵ��� �̵��� �ֱ� ���� �Լ�
UFUNCTION(BlueprintCallable)
void ApplyMovementValues();

UFUNCTION(BlueprintCallable)
void OnSpeedTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

UFUNCTION(BlueprintCallable)
void OnDistanceTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
TSubclassOf<UUserWidget> ActionChoice;

// ������Ʈ ���ʸ� �������� �Լ� ����
AActor* GetOwnerFromComponent();

// �̵� ������ ����
FVector StoredMoveDirection;

UFUNCTION(BlueprintCallable)
void InitializeFunctionObjects();

UFUNCTION(BlueprintCallable)
void AddObjectToComboBox(AActor* NewObject);

UFUNCTION(BlueprintCallable)
void OnFunctionObjectSelected(FString SelectedItem, ESelectInfo::Type SelectionType);

UFUNCTION()
UWidget* GenerateComboBoxItem(FString Item);

//-----------------------------------------------------------------------------------------------
// �պ����� �Լ���
UFUNCTION(BlueprintCallable)
void OnLoopModeCheckChanged(bool bIsChecked); // üũ�ڽ� ���� ���� �� ȣ��

UFUNCTION(BlueprintCallable)
void OnSingleDirectionCheckChanged(bool bIsChecked);  // �ܼ� �̵� ��� üũ�ڽ� ���� ����

//------------------------------------------------------------------------------------------------

// Array to store multiple MoveActor components
UPROPERTY()
TArray<UMyMoveActorComponent*> ControlledMoveComponents;

// Add a move component to the widget's control list
void AddControlledMoveComponent(UMyMoveActorComponent* MoveComponent);

void SetMoveDirection(FVector Direction);

UPROPERTY()
TMap<UMyMoveActorComponent*, FVector> MoveDirectionMap;

UPROPERTY()
TMap<UMyMoveActorComponent*, float> MoveSpeedMap;

//----------------------------------------------------------------------------------------------
   // Preview Actor ���� ���� �� �Լ�
AActor* PreviewActor = nullptr;           // ������ ������Ʈ
void SpawnPreviewActor();                 // ������ ���� ���� �Լ�
void UpdatePreviewLocation(FVector Direction, float Distance);  // ������ ��ġ ������Ʈ
void DestroyPreviewActor();               // ������ ���� ����
void UpdatePreviewMovement();

FTimerHandle PreviewMoveTimer;  // ������ ���� �̵� Ÿ�̸�
float StoredMoveSpeed;			// �̵� �ӵ�
float StoredMoveDistance;		// �̵� �Ÿ�

// ���� ��ȯ ���� (�� ���� ���� �߰� �ʿ�)
bool bPreviewDirectionReversed = false;   // �̵� ���� ���� ����

};