// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/ServerListAndDetail/ServerListAndDetail.h"
#include "CSR/UI/ServerListAndDetail/ServerListItem/OpenServerList.h"
#include "CSR/UI/ServerListAndDetail/ServerListsWidget.h"
#include "CSR/UI/ServerListAndDetail/RoomDetailPanel.h"
#include "PSH/PSH_Player/PSH_PlayerController.h"

void UServerListAndDetail::NativeConstruct()
{
	Super::NativeConstruct();
	APSH_PlayerController *player = Cast<APSH_PlayerController>(GetWorld()->GetFirstPlayerController());
	player->ListsWidget = this;
}

void UServerListAndDetail::SendDetailData(const FString& RoomName, const FString& Title, int32 CurrentPlayer)
{
	this->Play_Detail->MakeDetailPanel(RoomName, Title, CurrentPlayer);
}
