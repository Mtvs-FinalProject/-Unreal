// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/PSH_Component/PSH_SnapPoint.h"
#include "PSH/PSH_Actor/PSH_BlockActor.h"

void UPSH_SnapPoint::BeginPlay()
{
	Super::BeginPlay();

	auto * block = Cast<APSH_BlockActor>(GetOwner());

	if (block)
	{
		block->AddSnapPoint(GetRelativeLocation(),GetRelativeRotation(), priority);
		DataSyncComplete();
	}
}
void UPSH_SnapPoint::DataSyncComplete()
{
	DestroyComponent(true);
}