// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/ServerListAndDetail/ServerListItem/OpenServerItem.h"
#include "PSH/PSH_Player/PSH_PlayerController.h"
#include "CSR/UI/ServerListAndDetail/ServerListItem/OpenServerList.h"
#include "Styling/SlateTypes.h"
#include "Components/PanelWidget.h"
#include "CSR/UI/ServerListAndDetail/ServerListsWidget.h"
#include "CSR/UI/ServerListAndDetail/RoomDetailPanel.h"

UOpenServerItem::UOpenServerItem(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
    this->bIsSelected = false;
}

void UOpenServerItem::NativeConstruct()
{
    Super::NativeConstruct();

    if (MainButton)
    {
        MainButton->OnClicked.AddDynamic(this, &UOpenServerItem::OnMainButtonClicked);
    }

}

void UOpenServerItem::SetRoomInfo(const FRoomListInfo& InRoomInfo)
{
    this->CurrentRoomName = InRoomInfo.RoomName;
    this->CurrentMode = InRoomInfo.RoomMode;
    this->SelectedMap = InRoomInfo.SelectedMap;
    this->IsPlay = InRoomInfo.bIsPlaying;
    this->CurrentPlayer = InRoomInfo.CurrentPlayers;

    if (RoomNameText)
    {
        RoomNameText->SetText(FText::FromString(InRoomInfo.RoomName));
    }

    if (ModeText)
    {
        FString ModeString = (CurrentMode == ERoomMode::Create) ? TEXT("Create") : TEXT("Play");
        ModeText->SetText(FText::FromString(ModeString));
    }

    if (PlayerCountText)
    {
        FString PlayerCount = FString::Printf(TEXT("%d/%d"),
            InRoomInfo.CurrentPlayers, InRoomInfo.MaxPlayers);
        PlayerCountText->SetText(FText::FromString(PlayerCount));
    }
}

void UOpenServerItem::SetSelected(bool bSelected)
{
    bIsSelected = bSelected;

    FLinearColor NewColor = bSelected ? SelectedColor : NormalColor;
    if (MainButton)
    {
		FButtonStyle NewStyle = MainButton->GetStyle(); // WidgetStyle 대신 GetStyle() 사용
		NewStyle.Normal.TintColor = NewColor;
		NewStyle.Hovered.TintColor = NewColor * 1.1f;
		NewStyle.Pressed.TintColor = NewColor * 0.9f;
		MainButton->SetStyle(NewStyle); // 수정된 스타일을 다시 적용
    }
}

void UOpenServerItem::OnMainButtonClicked()
{
    ParentWidget->SelectItem(this);
    UE_LOG(LogTemp, Warning, TEXT("csr %s"), *this->SelectedMap);
    ParentWidget->SendDetailData(this->CurrentRoomName, this->SelectedMap, this->CurrentPlayer);
}
