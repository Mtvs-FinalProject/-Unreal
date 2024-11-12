// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyRotateActorComponent.h"

#include "RotationWidget.generated.h"

// ȸ�� �ӵ� �� ������Ʈ�� �˸��� �̺�Ʈ ��������Ʈ ����
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRotateSpeedMapUpdated);

UCLASS()
class FINALPROJECT_API URotationWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // UI �ʱ�ȭ
    virtual void NativeConstruct() override;

    // ȸ�� �ӵ� �Է� �ʵ�
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UEditableText* RotateSpeedText;

    // ȸ�� Ƚ���� �Է¹޴� �ؽ�Ʈ
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UEditableText* TimesRotateText;

    // ȸ�� �� ��ư
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* Btn_X;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* Btn_Y;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* Btn_Z;

    // UI ���� ��ư
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* Btn_RotateBack;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* Btn_RotateStart;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* Btn_RotateStop;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* Btn_RotateOrigin;

    // ȸ�� ���� üũ�ڽ�
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UCheckBox* RotateLoop;

    // Action Choice ���� Ŭ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
    TSubclassOf<UUserWidget> ActionChoice;

    // �޺��ڽ�����Ʈ (ȸ�� ������Ʈ ����)
    UPROPERTY(meta = (BindWidget))
    class UComboBoxString* RotateBoxList;

    // ȸ�� �� ��ư Ŭ�� �Լ�
    UFUNCTION(BlueprintCallable)
    void OnXClicked();
    UFUNCTION(BlueprintCallable)
    void OnYClicked();
    UFUNCTION(BlueprintCallable)
    void OnZClicked();

    // ȸ�� ���� ��ư Ŭ�� �Լ�
    UFUNCTION(BlueprintCallable)
    void OnRotateBackClicked();
    UFUNCTION(BlueprintCallable)
    void OnRotateStartClicked();
    UFUNCTION(BlueprintCallable)
    void OnRotateStopClicked();
    UFUNCTION(BlueprintCallable)
    void OnRotateOriginClicked();

    // ȸ�� ��� ȸ�� �ӵ� ���� �Լ�
    UFUNCTION(BlueprintCallable)
    void SetRotationDirection(FRotator NewDirection);

    // ȸ�� �ӵ� �Է� ó��
    UFUNCTION(BlueprintCallable)
    void OnRotateSpeedTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

    // ���� ��� üũ�ڽ� ���� ����
    UFUNCTION(BlueprintCallable)
    void OnLoopModeCheckChanged(bool bIsChecked);

    // ������Ʈ ��������, �ʱ�ȭ �� �޺��ڽ� ����
    UFUNCTION(BlueprintCallable)
    void InitializeFunctionObjects();
    UFUNCTION(BlueprintCallable)
    void AddObjectToComboBox(AActor* NewObject);
    UFUNCTION(BlueprintCallable)
    void OnFunctionObjectSelected(FString SelectedItem, ESelectInfo::Type SelectionType);

    // ���� ȸ�� ������Ʈ �迭�� �߰� �Լ�
    TArray<UMyRotateActorComponent*> ControlledRotateComponents;
    void AddControlledRotateComponent(UMyRotateActorComponent* NewComponent);

    // Ƚ�� �ؽ�Ʈ�� ����� �� ȣ��Ǵ� �Լ�
    UFUNCTION(BlueprintCallable)
    void UpdateTimesTextUI(const FText& Text, ETextCommit::Type CommitMethod);

    // ������Ʈ ���� ������Ʈ
    UFUNCTION(BlueprintCallable)
    void UpdateComponentSettings(UMyRotateActorComponent* RotateComponent);

    // ȸ�� ����
    UFUNCTION()
    void StartRotation();

    bool bIsInitialized = false; // ���� �ʱ�ȭ ���� �÷���
    void RetryInitialization();
    FTimerHandle TimerHandle;

    // ���� ���õ� ���� �� ��� ȸ�� ������Ʈ �迭
    AActor* SelectedActor;
    TArray<AActor*> AllFunctionObject;

    // ������Ʈ ���ʸ� �������� �Լ�
    AActor* GetOwnerFromComponent();

    // ȸ�� ����, �ӵ� ���� ������ ��
    TMap<UMyRotateActorComponent*, float> RotateSpeedMap;
    TMap<UMyRotateActorComponent*, int32> TimesRotateMap;
    TMap<UMyRotateActorComponent*, FRotator> RotateDirectionMap;

    // RotateSpeedMap ������Ʈ �̺�Ʈ
    UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
    FOnRotateSpeedMapUpdated OnRotateSpeedMapUpdated;

    // RotateSpeedMap ������Ʈ �� ȣ��Ǵ� �Լ�
    void UpdateRotateSpeedMap();
};