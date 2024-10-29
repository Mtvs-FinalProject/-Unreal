// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PSH_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API APSH_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APSH_PlayerController();

	virtual void PlayerTick(float DeltaTime) override;

	virtual void BeginPlay() override;

	void LookMouseCursor();

	bool targetLockOn = false;

	int32 RowNum = 0;
	
	UPROPERTY(EditAnywhere)
	class UDataTable * dataTable;

	//Widgets 
	void ObjectSave();
	void ObjectLoad();

	// ���õ� ������Ʈ�� ������ ui ���� �Լ�
	UFUNCTION()
	void SelectObject(AActor* SelectedActor);

	UPROPERTY()
	class APSH_Player * curPlayer;

	// URotationwidget Ŭ������ ���� ������ (UI �ν��Ͻ�)
	UPROPERTY()
	class URotationWidget* RotationWidget;

	//UMyChoiceActionWidget Ŭ������ ���� ������(UI �ν��Ͻ�)
	UPROPERTY()
	class UMyChoiceActionWidget* MyChoiceActionWidget;

	UPROPERTY()
    TSubclassOf<UUserWidget> ChoiceWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UCSR_Proto_StartUI> StartUIFac; 

	UPROPERTY()
	class UCSR_Proto_StartUI *StartUI;

	UFUNCTION(BlueprintImplementableEvent)
	void SaveTheGame();
};
