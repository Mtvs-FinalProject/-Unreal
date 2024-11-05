// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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

// ALLFunctionObject: 생성된 기능 오브젝트들을 저장하는 배열
UPROPERTY()
TArray<AActor*> AllFunctionObject;

// ComboBox Widget
UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
class UComboBoxString* MoveBoxList;

// 현재 선택된 오브젝트
AActor* SelectedActor = nullptr;

//------------------------------------------------------------------------------------------------------------------------------------------
UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UEditableText* SpeedMoveText;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UEditableText* DistMoveText;
//------------------------------------------------------------------------------------------------------------------------------------------


UFUNCTION()
void OnFrontClicked();

UFUNCTION()
void OnBackClicked();

UFUNCTION()
void OnRightClicked();

UFUNCTION()
void OnLeftClicked();

UFUNCTION()
void OnMoveBackClicked();

UFUNCTION()
void OnStartButtonClicked();

UFUNCTION()
void OnStopButtonClicked();

UFUNCTION()
void OnOriginButtonClicked();

//속도값 SetText로 설정하기
UFUNCTION()
void UpdateMovementValuesInUI(float SpeedValue, float DistanceValue);

// 속도나 이동값 넣기 위한 함수
UFUNCTION()
void ApplyMovementValues();

UFUNCTION()
void OnSpeedTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

UFUNCTION()
void OnDistanceTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
TSubclassOf<UUserWidget> ActionChoice;

// 컴포넌트 오너를 가져오는 함수 선언
AActor* GetOwnerFromComponent();

// 이동 저장할 변수
FVector StoredMoveDirection;

UFUNCTION()
void InitializeFunctionObjects();

UFUNCTION()
void AddObjectToComboBox(AActor* NewObject);

UFUNCTION()
void OnFunctionObjectSelected(FString SelectedItem, ESelectInfo::Type SelectionType);

};
