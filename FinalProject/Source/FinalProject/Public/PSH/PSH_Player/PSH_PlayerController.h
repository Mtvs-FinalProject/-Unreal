// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CSR/DedicatedServer/AutoRoomLevelInstance.h"
#include "CSR/HTTP/CSR_HTTP_Actor.h"
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
	UFUNCTION(BlueprintCallable)
	void ObjectSave();

	UFUNCTION(BlueprintCallable)
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

	UFUNCTION(Client,Reliable)
	void CRPC_CloseObjectWidget();

	UFUNCTION(Client,Reliable)
	void CRPC_ObjectWidgetVisible(bool check);

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

	UFUNCTION(BlueprintImplementableEvent)
	void SaveTheGame();

// 서버에 룸 관련 요청 - 성락
#pragma region
public:
    // 방 생성 관련 RPC 함수
    UFUNCTION(Server, Reliable)
    void ServerRequestCreateAutoRoom(const FString& RoomName, const FString& JsonData = TEXT(""));

    UFUNCTION(Server, Reliable)
    void ServerRequestJoinAutoRoom(const FString& RoomName);

    UFUNCTION(Server, Reliable)
    void ServerRequestLeaveAutoRoom(const FString& RoomName);

protected:
    // ObjectLoad 로직을 분리하여 재사용 가능한 함수로 만듦
    TArray<FPSH_ObjectData*> ParseJsonToObjectData(const FString& JsonString);
#pragma endregion
public:
	UPROPERTY()
	class UMainPageWidget* MainUI;

	UFUNCTION(Client, Reliable)
    void Client_ShowMainUI();

	UFUNCTION(Server, Reliable)
    void ServerRequestCleanupCurrentRoom();
private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UMainPageWidget> MainUIClass;
// 맵 저장 필요 로직 - 성락
#pragma region
    //// HTTP Actor 참조
    //UPROPERTY()
    //ACSR_HTTP_Actor* HTTPActor;

	//UPROPERTY()
	//FString CurrentLevel = "Main";

    // 맵 저장 UI 팩토리
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UWBP_CreateWidget> CreateWidgetClass;

 //   // 맵 저장 UI 인스턴스
 //   UPROPERTY()
 //   class UWBP_CreateWidget* MapCreateWidget;

	//// 맵 저장 UI 팩토리
 //   UPROPERTY(EditDefaultsOnly, Category = "UI")
 //   TSubclassOf<class UWBP_CreateWidget> MainWidgetClass;

 //   // UI 표시 함수
 //   UFUNCTION(BlueprintCallable)
 //   void ShowMapSaveUI();
#pragma endregion

};
