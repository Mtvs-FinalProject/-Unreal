// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyRotateActorComponent.h"

#include "RotationWidget.generated.h"

// 회전 속도 맵 업데이트를 알리는 이벤트 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRotateSpeedMapUpdated);

UCLASS()
class FINALPROJECT_API URotationWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // UI 초기화
    virtual void NativeConstruct() override;

    // 회전 속도 입력 필드
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UEditableText* RotateSpeedText;

    // 회전 횟수를 입력받는 텍스트
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UEditableText* TimesRotateText;

    // 회전 축 버튼
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* Btn_X;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* Btn_Y;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* Btn_Z;

    // UI 제어 버튼
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* Btn_RotateBack;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* Btn_RotateStart;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* Btn_RotateStop;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* Btn_RotateOrigin;

    // 회전 루프 체크박스
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UCheckBox* RotateLoop;

    // Action Choice 위젯 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
    TSubclassOf<UUserWidget> ActionChoice;

    // 콤보박스리스트 (회전 오브젝트 선택)
    UPROPERTY(meta = (BindWidget))
    class UComboBoxString* RotateBoxList;

    // 회전 축 버튼 클릭 함수
    UFUNCTION(BlueprintCallable)
    void OnXClicked();
    UFUNCTION(BlueprintCallable)
    void OnYClicked();
    UFUNCTION(BlueprintCallable)
    void OnZClicked();

    // 회전 제어 버튼 클릭 함수
    UFUNCTION(BlueprintCallable)
    void OnRotateBackClicked();
    UFUNCTION(BlueprintCallable)
    void OnRotateStartClicked();
    UFUNCTION(BlueprintCallable)
    void OnRotateStopClicked();
    UFUNCTION(BlueprintCallable)
    void OnRotateOriginClicked();

    // 회전 축과 회전 속도 설정 함수
    UFUNCTION(BlueprintCallable)
    void SetRotationDirection(FRotator NewDirection);

    // 회전 속도 입력 처리
    UFUNCTION(BlueprintCallable)
    void OnRotateSpeedTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

    // 루프 모드 체크박스 상태 변경
    UFUNCTION(BlueprintCallable)
    void OnLoopModeCheckChanged(bool bIsChecked);

    // 오브젝트 가져오기, 초기화 및 콤보박스 설정
    UFUNCTION(BlueprintCallable)
    void InitializeFunctionObjects();
    UFUNCTION(BlueprintCallable)
    void AddObjectToComboBox(AActor* NewObject);
    UFUNCTION(BlueprintCallable)
    void OnFunctionObjectSelected(FString SelectedItem, ESelectInfo::Type SelectionType);

    // 다중 회전 컴포넌트 배열과 추가 함수
    TArray<UMyRotateActorComponent*> ControlledRotateComponents;
    void AddControlledRotateComponent(UMyRotateActorComponent* NewComponent);

    // 횟수 텍스트가 변경될 때 호출되는 함수
    UFUNCTION(BlueprintCallable)
    void UpdateTimesTextUI(const FText& Text, ETextCommit::Type CommitMethod);

    // 컴포넌트 설정 업데이트
    UFUNCTION(BlueprintCallable)
    void UpdateComponentSettings(UMyRotateActorComponent* RotateComponent);

    UFUNCTION()
	UWidget* GenerateComboBoxItem(FString Item);

    // 회전 시작
    UFUNCTION()
    void StartRotation();

    bool bIsInitialized = false; // 위젯 초기화 상태 플래그
    void RetryInitialization();
    FTimerHandle TimerHandle;

    // 현재 선택된 액터 및 모든 회전 오브젝트 배열
    AActor* SelectedActor;
    TArray<AActor*> AllFunctionObject;

    // 컴포넌트 오너를 가져오는 함수
    AActor* GetOwnerFromComponent();

    // 회전 방향, 속도 등을 저장할 맵
    TMap<UMyRotateActorComponent*, float> RotateSpeedMap;
    TMap<UMyRotateActorComponent*, int32> TimesRotateMap;
    TMap<UMyRotateActorComponent*, FRotator> RotateDirectionMap;

    // RotateSpeedMap 업데이트 이벤트
    UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
    FOnRotateSpeedMapUpdated OnRotateSpeedMapUpdated;

    // RotateSpeedMap 업데이트 시 호출되는 함수
    void UpdateRotateSpeedMap();
};