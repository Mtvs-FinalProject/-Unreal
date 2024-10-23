// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RotationWidget.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API URotationWidget : public UUserWidget
{
	GENERATED_BODY()

public:

UFUNCTION()
virtual void NativeConstruct() override;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UEditableText* SpeedRotateText;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_X;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_Y;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_Z;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UCheckBox* RotateLoop;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_RotateBack;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_RotateStart;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_RotateStop;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_RotateOrigin;

UFUNCTION()
void OnXClicked();

UFUNCTION()
void OnYClicked();

UFUNCTION()
void OnZClicked();

UFUNCTION()
void OnRotateBackClicked();

UFUNCTION()
void OnRotateStartClicked();

UFUNCTION()
void OnRotateOriginClicked();

UFUNCTION()
void OnRotateStopClicked();

// �ӵ� �ؽ�Ʈ �ֱ�
UFUNCTION()
void UpdateSpeedTextUI(const FText& Text, ETextCommit::Type CommitMethod);


UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
TSubclassOf<UUserWidget> ActionChoice;

// ������Ʈ ���ʸ� �������� �Լ� ����
AActor* GetOwnerFromComponent();

};
