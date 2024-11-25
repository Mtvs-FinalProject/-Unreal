// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelInstance/LevelInstanceActor.h"
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
#pragma region 

    // 방제목 설정 및 가져오기
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Room")
    FString RoomName;

    // 방제목 설정 함수
    void SetRoomName(const FString& NewRoomName) { RoomName = NewRoomName; }

    const FString& GetRoomName() const { return (RoomName); };

    UFUNCTION(Server, Reliable)
    void ServerSpawnActorsFromJson(const FString& JsonString);

	UFUNCTION(Server, Reliable)
    void ServerAddPlayerToRoom(APlayerController* PlayerController);

    UFUNCTION(Server, Reliable)
    void ServerRemovePlayerFromRoom(APlayerController* PlayerController);

private:
    UPROPERTY(ReplicatedUsing = OnRep_ManagedActors)
    TArray<AActor*> ManagedActors;

    UPROPERTY(Replicated)
    TArray<APlayerController*> PlayersInRoom;

    UFUNCTION()
    void OnRep_ManagedActors();

    void SpawnActorsFromJson(const FString& JsonString);
    void SpawnPlayerCharacter(APlayerController* PlayerController);

    virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
#pragma endregion
public:
    // 방 할당 여부
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
    bool bIsRoomAssigned;

    UFUNCTION(BlueprintCallable, Category = "Room")
    bool IsPlayerInRoom(APlayerController* PlayerController) const;

    // 현재 방에 있는 플레이어 컨트롤러들을 추적
    UPROPERTY(ReplicatedUsing = OnRep_ConnectedPlayers, BlueprintReadOnly, EditAnywhere)
    TArray<APlayerController*> ConnectedPlayers;

    // Runtime WorldAsset 관리
    UPROPERTY(ReplicatedUsing = OnRep_RuntimeWorldAsset)
    TSoftObjectPtr<UWorld> RuntimeWorldAsset;

	UPROPERTY(Replicated)
	FString CurrentRoomName;

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

    // 서버에서만 호출되는 방 관리 함수들
    UFUNCTION(Server, Reliable) 
    void ServerAssignAutoRoom(const FString& NewRoomName);

    UFUNCTION(Server, Reliable)
    void ServerUnassignAutoRoom();
 
    UFUNCTION(Server, Reliable)
    void ServerJoinRoom(APlayerController* JoiningPlayer);

    UFUNCTION(Server, Reliable)
    void ServerLeaveRoom(APlayerController* LeavingPlayer);

	UFUNCTION(Client, Reliable)
	void ClientOnLeaveRoom(APlayerController* LeavingPlayer);

    // WorldAsset 관련 오버라이드
    UFUNCTION()
    bool SetRuntimeWorldAsset(TSoftObjectPtr<UWorld> InWorldAsset);

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnRep_RuntimeWorldAsset();

    UFUNCTION()
    void OnRep_ConnectedPlayers();

    // WorldAsset이 변경될 때 호출될 함수
    void HandleWorldAssetChanged();
    bool ShouldLoadLevelLocally() const;

    UFUNCTION()
    void OnRep_RoomState();

    // 각 클라이언트의 로컬 레벨 로드 상태
    UPROPERTY(Transient)
    bool bIsLevelLoadedLocally;

    // WorldAsset 경로
    UPROPERTY()
    FString LevelPath;	


// 추가 코드
#pragma region
public:
    UPROPERTY(Replicated)
    APlayerController* RoomCreator;

    // 저장 권한 체크 함수
    UFUNCTION(BlueprintPure, Category="Room")
    bool HasSavePermission(APlayerController* PlayerController) const
    {
        return RoomCreator == PlayerController;
    }

protected:


private:


#pragma endregion

};
