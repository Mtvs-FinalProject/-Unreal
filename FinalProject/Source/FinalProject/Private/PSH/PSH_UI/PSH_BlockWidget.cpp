// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/PSH_UI/PSH_BlockWidget.h"
#include "Components/Button.h"
#include "PSH/PSH_Player/PSH_Player.h"
#include "PSH/PSH_Actor/PSH_BlockActor.h"

void UPSH_BlockWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Icon->OnClicked.AddDynamic(this,&UPSH_BlockWidget::SpawnBlock);
}

void UPSH_BlockWidget::SpawnBlock()
{
	APSH_Player * player = Cast<APSH_Player>(GetWorld()->GetFirstPlayerController()->GetCharacter());

	if(player == nullptr) return;
	FActorSpawnParameters parm;

	player->SetPreviewMesh(spawnMesh, spawnActor);

	//player->SRPC_SpawnBlock(spawnActor); // 청사진 소환
}