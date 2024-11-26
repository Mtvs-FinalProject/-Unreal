// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ActionChoice.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UActionChoice : public UUserWidget
{
	GENERATED_BODY()
	

public:
	
UFUNCTION()
virtual void NativeConstruct() override;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_Move;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_Rotate;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* Btn_Fly;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
class UButton* End_Button;

UFUNCTION()
void OnMoveClicked();

UFUNCTION()
void OnRotateClicked();

UFUNCTION()
void OnEndClicked();

UFUNCTION()
void OnFlyClicked();

// ���͸� ������ �� �ִ� �Լ�
UFUNCTION()
void SetControlledActor(AActor* NewActor);

// ó��ȭ������ ���ư���
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
TSubclassOf<UUserWidget> FirstSelect;

// ȸ������ ����
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
TSubclassOf<UUserWidget> WBP_Rotate;

// �̵����� ����
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
TSubclassOf<UUserWidget> WBP_Move;

// ����� ����
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
TSubclassOf<UUserWidget> WBP_Fly;

// ��� ���Ͽ� ���͸� ������ ������ ����
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActionChoice")
AActor* ControlledActor = nullptr;




};
