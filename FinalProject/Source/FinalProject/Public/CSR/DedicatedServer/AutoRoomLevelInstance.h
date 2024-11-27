// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelInstance/LevelInstanceActor.h"
#include "AutoGameState.h"
#include "AutoRoomLevelInstance.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API AAutoRoomLevelInstance : public ALevelInstance
{
	GENERATED_BODY()
public:
    AAutoRoomLevelInstance();

public:
    // 방 할당 여부
    UPROPERTY(Replicated)   
    bool bIsRoomAssigned;

    // 현재 방에 있는 플레이어 컨트롤러들을 추적
    UPROPERTY(ReplicatedUsing = OnRep_ConnectedPlayers, BlueprintReadOnly, EditAnywhere)
    TArray<APlayerController*> ConnectedPlayers;

    // Runtime WorldAsset 관리
    UPROPERTY(ReplicatedUsing = OnRep_RuntimeWorldAsset)
    TSoftObjectPtr<UWorld> RuntimeWorldAsset;

	UPROPERTY(Replicated)
	FString CurrentRoomName = "";
    
    UPROPERTY(Replicated)
    FString SelectedMap = "";

	UPROPERTY(Replicated)
	ERoomMode RoomMode = ERoomMode::NoMode;

	UFUNCTION(BlueprintCallable, Category = "Room")
	FString GetCurrentRoomName() const {
		return CurrentRoomName;
	}

	UFUNCTION(BlueprintCallable, Category = "Room")
	void SetCurrentRoomName(const FString& NewRoomName) {
		if (HasAuthority()) {
			CurrentRoomName = NewRoomName;
		}
	}

    UFUNCTION(BlueprintCallable, Category = "Room")
	FString GetSelectedMap() const {
		return SelectedMap;
	}

	UFUNCTION(BlueprintCallable, Category = "Room")
	void SetSelectedMap(const FString& SelectedMap_) {
		if (HasAuthority()) {
            SelectedMap = SelectedMap_;
		}
	}

    // 서버에서만 호출되는 방 관리 함수들
	UFUNCTION(Server, Reliable)
	void ServerAssignAutoRoom(const FString& NewRoomName, const FString& JsonData);

    UFUNCTION(Server, Reliable)
    void ServerUnassignAutoRoom();
 
    UFUNCTION(Server, Reliable)
    void ServerGetOutAll();

    UFUNCTION(Server, Reliable)
    void ServerJoinRoom(APlayerController* JoiningPlayer);

    UFUNCTION(Server, Reliable)
    void ServerLeaveRoom(APlayerController* LeavingPlayer);

    // 클라이언트 통지
    UFUNCTION(Client, Reliable)
    void ClientOnLeaveRoom(APlayerController* LeavingPlayer);

    // WorldAsset 관련 오버라이드
    UFUNCTION()
    bool SetRuntimeWorldAsset(TSoftObjectPtr<UWorld> InWorldAsset);

    // 특정 플레이어가 이 방에 있는지 확인
    bool IsPlayerInRoom(APlayerController* PlayerController) const;

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // 방에 접속한 모든 플레이어를 로비로 되돌리는 함수
    void ReturnAllPlayersToLobby();

    // WorldAsset이 변경될 때 호출될 함수
    void HandleWorldAssetChanged();
    bool ShouldLoadLevelLocally() const;

    UFUNCTION()
    void OnRep_RuntimeWorldAsset();

    UFUNCTION()
    void OnRep_ConnectedPlayers();

    UFUNCTION()
    void OnRep_RoomState();

    // 각 클라이언트의 로컬 레벨 로드 상태
    UPROPERTY(Transient)
    bool bIsLevelLoadedLocally;

    // WorldAsset 경로
    UPROPERTY()
    FString LevelPath;	

// 레벨 스폰 관련
#pragma region 
    public:
	// ILevelInstanceInterface 오버라이드
	virtual void OnLevelInstanceLoaded() override;
    private:
		// JSON 데이터 저장
		UPROPERTY()
		FString PendingJsonData;

		// JSON 데이터로부터 액터 스폰
		void SpawnActorsFromJson();

		// 캐릭터 스폰 및 설정
		void SpawnAndSetupCharacter(APlayerController* PlayerController);

        // 모든 엑터를 추적하여 삭제한다.
        void CleanupSpawnedActors();

        // 스폰된 액터들 추적
		UPROPERTY()
		TArray<AActor*> SpawnedActors;

#pragma endregion

};