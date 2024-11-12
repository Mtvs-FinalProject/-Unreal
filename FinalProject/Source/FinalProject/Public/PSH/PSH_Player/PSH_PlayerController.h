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

	// 선택된 오브젝트와 연동된 ui 연결 함수
	UFUNCTION()
	void SelectObject(AActor* SelectedActor);

	//Widgets 
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UUserWidget> objectWidgetFac;
	UPROPERTY()
	class UPSH_ObjectWidget * objectWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UUserWidget> mouseWidgetFac;
	UPROPERTY()
	class UPSH_MouseWidget* mouseWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UUserWidget> garbageBotWidgetFac;
	UPROPERTY()
	class UPSH_GarbageBotWidget* garbageBotWidget;

	UFUNCTION(Client,Reliable)
	void CRPC_ShowObjectWidget();

	UPROPERTY()
	class APSH_Player * curPlayer;

	// URotationwidget 클래스에 대한 포인터 (UI 인스턴스)
	UPROPERTY()
	class URotationWidget* RotationWidget;

	//UMyChoiceActionWidget 클래스에 대한 포인터(UI 인스턴스)
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
