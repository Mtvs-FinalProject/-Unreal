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

    // ������ ���� �� ��������
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Room")
    FString RoomName;

    // ������ ���� �Լ�
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
    // �� �Ҵ� ����
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
    bool bIsRoomAssigned;

    UFUNCTION(BlueprintCallable, Category = "Room")
    bool IsPlayerInRoom(APlayerController* PlayerController) const;

    // ���� �濡 �ִ� �÷��̾� ��Ʈ�ѷ����� ����
    UPROPERTY(ReplicatedUsing = OnRep_ConnectedPlayers, BlueprintReadOnly, EditAnywhere)
    TArray<APlayerController*> ConnectedPlayers;

    // Runtime WorldAsset ����
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

    // ���������� ȣ��Ǵ� �� ���� �Լ���
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

    // WorldAsset ���� �������̵�
    UFUNCTION()
    bool SetRuntimeWorldAsset(TSoftObjectPtr<UWorld> InWorldAsset);

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnRep_RuntimeWorldAsset();

    UFUNCTION()
    void OnRep_ConnectedPlayers();

    // WorldAsset�� ����� �� ȣ��� �Լ�
    void HandleWorldAssetChanged();
    bool ShouldLoadLevelLocally() const;

    UFUNCTION()
    void OnRep_RoomState();

    // �� Ŭ���̾�Ʈ�� ���� ���� �ε� ����
    UPROPERTY(Transient)
    bool bIsLevelLoadedLocally;

    // WorldAsset ���
    UPROPERTY()
    FString LevelPath;	


// �߰� �ڵ�
#pragma region
public:
    UPROPERTY(Replicated)
    APlayerController* RoomCreator;

    // ���� ���� üũ �Լ�
    UFUNCTION(BlueprintPure, Category="Room")
    bool HasSavePermission(APlayerController* PlayerController) const
    {
        return RoomCreator == PlayerController;
    }

protected:


private:


#pragma endregion

};
