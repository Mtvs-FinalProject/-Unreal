// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/PSH_GameMode/PSH_GameModeBase.h"

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