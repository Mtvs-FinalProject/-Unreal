// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/DedicatedServer/AutoGameState.h"
#include "Net/UnrealNetwork.h"
#include "CSR/DedicatedServer/AutoRoomManager.h"
#include "CSR/DedicatedServer/AutoRoomLevelInstance.h"

void AAutoGameState::UpdateRoomList()
{
    if (!HasAuthority() || !AutoRoomManager)
    {
        return;
	}
    RoomListInfo.Empty();
    UE_LOG(LogTemp, Warning, TEXT("csr Update"));
    // AutoRoomManager의 방 풀을 순회하며 정보 수집
    for (AAutoRoomLevelInstance* Room : AutoRoomManager->AutoRoomPool)
    {
        if (Room && Room->bIsRoomAssigned)
        {
            FRoomListInfo NewInfo;
            NewInfo.RoomName = Room->GetCurrentRoomName();
            NewInfo.SelectedMap = Room->GetSelectedMap();
            NewInfo.CurrentPlayers = Room->ConnectedPlayers.Num();
            NewInfo.MaxPlayers = 4; // 또는 설정된 최대 플레이어 수
            NewInfo.bIsPlaying = true; // 또는 방의 상태에 따라 설정
            NewInfo.RoomMode = Room->RoomMode;
            RoomListInfo.Add(NewInfo);
        }
    }

    // 서버에서 직접 델리게이트 호출
    OnRoomListUpdated.Broadcast();
}

void AAutoGameState::OnRep_RoomList()
{
    // 클라이언트에서 방 목록이 업데이트되면 델리게이트 호출
    OnRoomListUpdated.Broadcast();
}

void AAutoGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// AutoRoomManager 복제 설정을 Reliable로 설정
	DOREPLIFETIME_CONDITION_NOTIFY(AAutoGameState, AutoRoomManager, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME(AAutoGameState, RoomListInfo);
}
