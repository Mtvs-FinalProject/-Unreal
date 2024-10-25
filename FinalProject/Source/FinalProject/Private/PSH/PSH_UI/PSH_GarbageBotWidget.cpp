// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/PSH_UI/PSH_GarbageBotWidget.h"
#include "Components/Button.h"
#include "PSH/PSH_Actor/PSH_GarbageBot.h"
#include "Components/WidgetComponent.h"

void UPSH_GarbageBotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btr_Move->OnClicked.AddDynamic(this,&UPSH_GarbageBotWidget::OnClickMove);
	Btr_Destroy->OnClicked.AddDynamic(this, &UPSH_GarbageBotWidget::OnClickDestroy);
	Btr_Idle->OnClicked.AddDynamic(this, &UPSH_GarbageBotWidget::OnClickIdle);

	
}

void UPSH_GarbageBotWidget::OnClickMove()
{
	bot->SetState(EState::MOVE);
	SetVisibility(ESlateVisibility::Hidden);
	
}
void UPSH_GarbageBotWidget::OnClickDestroy()
{
	bot->SetState(EState::Destroy);
	SetVisibility(ESlateVisibility::Hidden);
}
void UPSH_GarbageBotWidget::OnClickIdle()
{
	bot->SetState(EState::IDLE);
	SetVisibility(ESlateVisibility::Hidden);
}
void UPSH_GarbageBotWidget::SetOwner(class APSH_GarbageBot* owner)
{
	bot = owner;
}