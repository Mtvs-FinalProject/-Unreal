// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AutoRoomManager.h"
#include "Delegates/DelegateCombinations.h"
#include "AutoGameState.generated.h"


class AutoRoomLevelInstance;

UENUM(BlueprintType)
enum class ERoomMode : uint8
{
    Play,
    Create,
    NoMode
};

USTRUCT(BlueprintType)
struct FRoomListInfo
{
    GENERATED_BODY()

    UPROPERTY()
    FString RoomName;
    
    UPROPERTY()
    int32 CurrentPlayers;
    
    UPROPERTY()
    int32 MaxPlayers;
    
    UPROPERTY()
    bool bIsPlaying;

    UPROPERTY()
    ERoomMode RoomMode;
};

/**
 * 
 */
UCLASS()
class FINALPROJECT_API AAutoGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
    // 기존 RoomManager
    UPROPERTY(Replicated)
    AAutoRoomManager* AutoRoomManager;

    // 새로 추가되는 방 목록 관련 기능
    UPROPERTY(ReplicatedUsing = OnRep_RoomList)
    TArray<FRoomListInfo> RoomListInfo;

    // 방 목록 업데이트 시 호출될 델리게이트
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoomListUpdated);

    UPROPERTY(BlueprintAssignable)
    FOnRoomListUpdated OnRoomListUpdated;

    // 서버에서 방 정보 업데이트
    void UpdateRoomList();

protected:
    UFUNCTION()
    void OnRep_RoomList();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
