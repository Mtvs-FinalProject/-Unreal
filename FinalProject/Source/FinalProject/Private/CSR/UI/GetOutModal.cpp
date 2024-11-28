// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/GetOutModal.h"
#include "Components/Button.h"
#include "PSH/PSH_Player/PSH_PlayerController.h"




void UGetOutModal::NativeConstruct()
{
	if (YesBTN) {
		YesBTN->OnClicked.AddDynamic(this, &UGetOutModal::OnClickYesBTN);
	}
	if (NoBTN) {
		NoBTN->OnClicked.AddDynamic(this, &UGetOutModal::OnClickNoBTN);
	}
}

void UGetOutModal::OnClickYesBTN()
{
	Player->ServerRequestLeaveCurrentAutoRoom();
	RemoveFromParent();
}

void UGetOutModal::OnClickNoBTN()
{
	RemoveFromParent();
}
