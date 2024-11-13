// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/PSH_GameMode/PSH_GameModeBase.h"
#include "../FinalProject.h"

AActor* APSH_GameModeBase::SpawnFunctionObject()
{	
	FStringClassReference BP_FunctionObjectClassRef(TEXT("/Game/YWK/BP/BP_MoveandFly.BP_MoveandFly_C"));
	UClass* BP_FunctionObjectClass = BP_FunctionObjectClassRef.TryLoadClass<AActor>();

	if (BP_FunctionObjectClass && GetWorld())
	{
		// ���ο� ��� ������Ʈ ����
		AActor* NewObject = GetWorld()->SpawnActor<AActor>(BP_FunctionObjectClass);

		if (NewObject)
		{
			// ������Ʈ ���� �� ��������Ʈ ȣ��
			OnObjectCreated.Broadcast(NewObject);
		}
		return NewObject;
	}
	return nullptr;
}
void APSH_GameModeBase::SetMode(ECreateMode changeMode)
{
	switch (mode)
	{
	case ECreateMode::PLAY:
		bCreateMode  = false;
		break;
	case ECreateMode::CREATE:
		bCreateMode  = true;
		break;
	}
}

ECreateMode APSH_GameModeBase::GetMode()
{
	return mode;

}

void APSH_GameModeBase::SetCreateingCheck(bool check)
{
	bCreateMode = check;
	MRPC_StartBlcok();
}

void APSH_GameModeBase::MRPC_StartBlcok_Implementation()
{
	if (onStartBlock.IsBound())
	{
		PRINTLOG(TEXT("Broadcast"));
		onStartBlock.Broadcast(bCreateMode); // ��������Ʈ ȣ��
	}
}

bool APSH_GameModeBase::GetCreateingCheck()
{
	return bCreateMode;
}