// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/ServerListAndDetail/ServerListItem/OpenServerItem.h"
#include "PSH/PSH_Player/PSH_PlayerController.h"
#include "CSR/UI/ServerListAndDetail/ServerListItem/OpenServerList.h"
#include "Styling/SlateTypes.h"
#include "Components/PanelWidget.h"

UOpenServerItem::UOpenServerItem(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , bIsSelected(false)
{
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
    CurrentRoomName = InRoomInfo.RoomName;
    CurrentMode = InRoomInfo.RoomMode;

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
        FButtonStyle NewStyle = MainButton->WidgetStyle;
        NewStyle.Normal.TintColor = NewColor;
        NewStyle.Hovered.TintColor = NewColor * 1.1f;
        NewStyle.Pressed.TintColor = NewColor * 0.9f;
        MainButton->SetStyle(NewStyle);
    }
}

void UOpenServerItem::OnMainButtonClicked()
{
    ParentWidget->SelectItem(this);
}
