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
	// 왕복 
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCheckBox* Chk_LoopMode; // 무한 왕복모드 체크 박스

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCheckBox* Chk_SingleDirectionMode; // 단순 이동 모드 체크박스

	// 왕복 횟수 입력 필드
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UEditableText* Txt_LoopCount;
	//------------------------------------------------------------------------------------------------

	// 현재 선택된 오브젝트
	AActor* SelectedActor = nullptr;

	// ALLFunctionObject: 생성된 기능 오브젝트들을 저장하는 배열
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

	// 컴포넌트 오너를 가져오는 함수 선언
	AActor* GetOwnerFromComponent();

	// 이동 저장할 변수
	FVector StoredFlyDirection;

	UFUNCTION(BlueprintCallable)
	void InitializeFunctionObjects();

	UFUNCTION(BlueprintCallable)
	void AddObjectToComboBox(AActor* NewObject);

	UFUNCTION(BlueprintCallable)
	void OnFunctionObjectSelected(FString SelectedItem, ESelectInfo::Type SelectionType);

	//------------------------------------------------------------------------------------
	// 왕복 함수
	UFUNCTION(BlueprintCallable)
	void OnLoopModeCheckChanged(bool bIsChecked); // 왕복 모드 체크박스 상태 변경

	UFUNCTION(BlueprintCallable)
	void OnSingleDirectionCheckChanged(bool bIsChecked); // 단순 이동 모드 체크박스 상태 변경

	UFUNCTION(BlueprintCallable)
    void OnLoopCountCommitted(const FText& Text, ETextCommit::Type CommitMethod);  // 왕복 횟수 입력 시 호출

	//--------------------------------------------------------------------------------------

	// 헤더 파일에 추가
	UPROPERTY()
	TArray<UMyFlyActorComponent*> ControlledFlyComponents; // 여러 FlyComponent를 제어할 배열

	UFUNCTION(BlueprintCallable)
	void AddControlledFlyComponent(UMyFlyActorComponent* NewFlyComponent);

	//-------------------------------------------------------------------------------------------
	   // Preview Actor 관련 변수 및 함수
	AActor* PreviewActor = nullptr;           // 프리뷰 오브젝트
	void SpawnPreviewActor();                 // 프리뷰 액터 스폰 함수
	void UpdatePreviewLocation(FVector Direction, float Distance);  // 프리뷰 위치 업데이트
	void UpdatePreviewMovement();
	void DestroyPreviewActor();               // 프리뷰 액터 삭제
	

	FTimerHandle PreviewFlyTimer;  // 프리뷰 액터 이동 타이머
	float StoredMoveSpeed;			// 이동 속도
	float StoredMoveDistance;		// 이동 거리

	// 방향 전환 여부 (이 변수 선언 추가 필요)
	bool bPreviewDirectionReversed = false;   // 이동 방향 반전 여부

	// BP_MoveandFly의 초기 위치 저장
	FVector MoveAndFlyInitialLocation;

	
};
