// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/PSH_UI/PSH_GarbageBotWidget.h"
#include "Components/Button.h"
#include "PSH/PSH_Actor/PSH_GarbageBot.h"
#include "Components/WidgetComponent.h"
#include "Animation/WidgetAnimationEvents.h"
#include "PSH/PSH_Player/PSH_Player.h"

void UPSH_GarbageBotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btr_Move->OnClicked.AddDynamic(this,&UPSH_GarbageBotWidget::OnClickMove);
	Btr_Destroy->OnClicked.AddDynamic(this, &UPSH_GarbageBotWidget::OnClickDestroy);
	Btr_Idle->OnClicked.AddDynamic(this, &UPSH_GarbageBotWidget::OnClickIdle);
	Btr_Bot->OnClicked.AddDynamic(this, &UPSH_GarbageBotWidget::OnClickBot);
	Btr_Bot->OnHovered.AddDynamic(this, &UPSH_GarbageBotWidget::OnHoveredBot);
	
	Btr_Move->SetVisibility(ESlateVisibility::Hidden);
	Btr_Destroy->SetVisibility(ESlateVisibility::Hidden);
	Btr_Idle->SetVisibility(ESlateVisibility::Hidden);
	// 애니메이션 끝났을때
// 	endDelegate.BindDynamic(this,&UPSH_GarbageBotWidget::HoverAnimEnd);
// 
// 	BindToAnimationFinished(ButtonHoverVisibleAnim, endDelegate);
}

void UPSH_GarbageBotWidget::OnClickMove()
{
	player = Cast<APSH_Player>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if(player == nullptr) return;
	player->SRPC_GarbageBotSetState(EState::MOVE);
	
	SetModeButtonUI(false);
}
void UPSH_GarbageBotWidget::OnClickDestroy()
{
	player = Cast<APSH_Player>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (player == nullptr) return;
	player->SRPC_GarbageBotSetState(EState::Destroy);
	
	SetModeButtonUI(false);
}
void UPSH_GarbageBotWidget::OnClickIdle()
{
	player = Cast<APSH_Player>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (player == nullptr) return;
	player->SRPC_GarbageBotSetState(EState::IDLE);
	
	SetModeButtonUI(false);
}

void UPSH_GarbageBotWidget::OnHoveredBot()
{
	bHoverdBot = !bHoverdBot;
	SetModeButtonUI(bHoverdBot);
}
void UPSH_GarbageBotWidget::OnClickBot()
{
	player = Cast<APSH_Player>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (player == nullptr) return;
	
	player->SRPC_GarbageBotInitPoint();
	
}
void UPSH_GarbageBotWidget::OnClickBackGround()
{
	bHoverdBot = false;
	PlayAnimationReverse(ButtonHoverHiddenAnim);
}

void UPSH_GarbageBotWidget::SetModeButtonUI(bool chek)
{
	if(!Btr_Move || !Btr_Destroy || !Btr_Idle) return;

	bHoverdBot = chek;
	if (bHoverdBot)
	{
		PlayAnimationForward(ButtonHoverVisibleAnim);
	}
	else
	{
		PlayAnimationReverse(ButtonHoverHiddenAnim);
	}
}