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

// 왕복 모드와 횟수 입력 위젯
UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UCheckBox* Chk_LoopMode;  // 무한 왕복 모드 체크박스

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UCheckBox* Chk_SingleDirectionMode;  // 단순 이동 모드 체크박스
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

//속도값 SetText로 설정하기
UFUNCTION(BlueprintCallable)
void UpdateMovementValuesInUI(float SpeedValue, float DistanceValue);

// 속도나 이동값 넣기 위한 함수
UFUNCTION(BlueprintCallable)
void ApplyMovementValues();

UFUNCTION(BlueprintCallable)
void OnSpeedTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

UFUNCTION(BlueprintCallable)
void OnDistanceTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
TSubclassOf<UUserWidget> ActionChoice;

// 컴포넌트 오너를 가져오는 함수 선언
AActor* GetOwnerFromComponent();

// 이동 저장할 변수
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
// 왕복관련 함수들
UFUNCTION(BlueprintCallable)
void OnLoopModeCheckChanged(bool bIsChecked); // 체크박스 상태 변경 시 호출

UFUNCTION(BlueprintCallable)
void OnSingleDirectionCheckChanged(bool bIsChecked);  // 단순 이동 모드 체크박스 상태 변경

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
   // Preview Actor 관련 변수 및 함수
AActor* PreviewActor = nullptr;           // 프리뷰 오브젝트
void SpawnPreviewActor();                 // 프리뷰 액터 스폰 함수
void UpdatePreviewLocation(FVector Direction, float Distance);  // 프리뷰 위치 업데이트
void DestroyPreviewActor();               // 프리뷰 액터 삭제
void UpdatePreviewMovement();

FTimerHandle PreviewMoveTimer;  // 프리뷰 액터 이동 타이머
float StoredMoveSpeed;			// 이동 속도
float StoredMoveDistance;		// 이동 거리

// 방향 전환 여부 (이 변수 선언 추가 필요)
bool bPreviewDirectionReversed = false;   // 이동 방향 반전 여부

};