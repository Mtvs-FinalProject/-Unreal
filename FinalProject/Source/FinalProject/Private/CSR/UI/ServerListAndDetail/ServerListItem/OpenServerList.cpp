// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/ServerListAndDetail/ServerListItem/OpenServerList.h"
#include "CSR/DedicatedServer/AutoGameState.h"
#include "CSR/UI/ServerListAndDetail/ServerListsWidget.h"
#include "Components/PanelSlot.h"
#include "CSR/UI/ServerListAndDetail/ServerListItem/OpenServerItem.h"

void UOpenServerList::NativeConstruct()
{
    Super::NativeConstruct();

    // GameState의 이벤트에 바인딩
    if (AAutoGameState* GS = GetWorld()->GetGameState<AAutoGameState>())
    {
        CachedGameState = GS;
        GS->OnRoomListUpdated.AddDynamic(this, &UOpenServerList::OnRoomListUpdated);
        // 초기 목록 표시
        RefreshRoomList();
    }
}

void UOpenServerList::NativeDestruct()
{
    // 이벤트 바인딩 해제
    if (CachedGameState.IsValid())
    {
        CachedGameState->OnRoomListUpdated.RemoveDynamic(this, &UOpenServerList::OnRoomListUpdated);
    }

    Super::NativeDestruct();
}

void UOpenServerList::OnRoomListUpdated()
{
    RefreshRoomList();
}

void UOpenServerList::RefreshRoomList()
{
    if (!RoomListBox || !ServerItemClass) return;

    // 선택된 아이템의 RoomName 저장
    FString SelectedRoomName;
    if (SelectedItem)
    {
        SelectedRoomName = SelectedItem->GetRoomName();
    }

    // 기존 항목들 제거
    RoomListBox->ClearChildren();
    SelectedItem = nullptr;

    // GameState에서 방 목록 가져와서 표시
    if (AAutoGameState* GS = GetWorld()->GetGameState<AAutoGameState>())
    {
        for (const FRoomListInfo& RoomInfo : GS->RoomListInfo)
        {
            if (UOpenServerItem* RoomItem = CreateWidget<UOpenServerItem>(this, ServerItemClass))
            {
                RoomItem->SetRoomInfo(RoomInfo);
                RoomItem->SetParentWidget(this);
                RoomListBox->AddChild(RoomItem);

                // 이전에 선택되었던 방이면 다시 선택
                if (!SelectedRoomName.IsEmpty() && SelectedRoomName == RoomInfo.RoomName)
                {
                    SelectItem(RoomItem);
                }
            }
        }
    }
}

void UOpenServerList::SelectItem(UOpenServerItem* NewSelectedItem)
{
    // 이전 선택 해제
    if (SelectedItem && SelectedItem != NewSelectedItem)
    {
        SelectedItem->SetSelected(false);
    }

    // 새로운 아이템 선택
    SelectedItem = NewSelectedItem;
    if (SelectedItem)
    {
        SelectedItem->SetSelected(true);
    }
}
