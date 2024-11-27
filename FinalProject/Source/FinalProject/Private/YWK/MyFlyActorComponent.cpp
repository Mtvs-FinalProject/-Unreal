// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/MyFlyActorComponent.h"
#include "PSH/PSH_Actor/PSH_BlockActor.h"
#include "../FinalProject.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UMyFlyActorComponent::UMyFlyActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// 초기 상승 속도
	FlySpeed = 0;

	// 초기 상승 상태
	bShouldFly = false;
	
	// 초기 상승 방향
	FlyDirection = FVector::ZeroVector;

	// 상승한 높이 초기화
	FlyDistance = 0.0f;

	bLoopMode = false;
	bSingleDirection = false;


	SetIsReplicated(true);
}

// Called when the game starts
void UMyFlyActorComponent::BeginPlay()
{
	Super::BeginPlay();

	APSH_BlockActor* block = Cast<APSH_BlockActor>(GetOwner());

	if (block)
	{
		block->componentCreateBoolDelegate.AddUObject(this,&UMyFlyActorComponent::GetDelegateBool);
	}

}


// Called every frame
void UMyFlyActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 로그로 bShouldFly 상태 확인
	//UE_LOG(LogTemp, Warning, TEXT("TickComponent: bShouldFly = %s"), bShouldFly ? TEXT("true") : TEXT("false"));

	if (bShouldFly)
	{
		objectFly(DeltaTime);
	}
}

void UMyFlyActorComponent::objectFly(float DeltaTime)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	PRINTLOG(TEXT("objectFly"));

	FVector NewLocation = Owner->GetActorLocation() + (FlyDirection * FlySpeed * DeltaTime);
	
	float DistanceTraveled = FVector::Dist(StartLocation, NewLocation);

	if (DistanceTraveled >= FlyDistance)
	{
		if (bSingleDirection)
		{
			StopFly();
		}
		else
		{
			// Loop mode: reverse direction and reset start location
			if (bLoopMode)
			{
				// Reverse direction
				FlyDirection *= -1.0f;
				StartLocation = Owner->GetActorLocation();

			}
			else
			{
				// Stop moving if loop count limit has been reached
				StopFly();
			}
		}
	}
	else
	{
		SRPC_SetOwnerLocation(NewLocation);
	}
}
void UMyFlyActorComponent::StartFly()
{
	StartLocation = GetOwner()->GetActorLocation();
	
	SRPC_SetOwnerSync(FlyDirection, FlyDistance,FlySpeed,bLoopMode,bSingleDirection);

	bShouldFly = true;

	APSH_BlockActor* block = Cast<APSH_BlockActor>(GetOwner());
	if (block && block->ActorHasTag(FName("owner")))
	{
		block->SaveBlockLocations();
		PRINTLOG(TEXT("SaveBlockLocations"));
	}

	if (block)
	{
		block->SRPC_SetSimulatePhysics(false);
	}

	UE_LOG(LogTemp, Warning, TEXT("Flight started with direction: %s"), *FlyDirection.ToString());
}

void UMyFlyActorComponent::StopFly()
{
	SRPC_StopFly();

	UE_LOG(LogTemp, Warning, TEXT("StopFly called. bShouldFly = %s"), bShouldFly ? TEXT("true") : TEXT("false"));
}

void UMyFlyActorComponent::SRPC_StopFly_Implementation()
{
	MRPC_StopFlay();

}
void UMyFlyActorComponent::MRPC_StopFlay_Implementation()
{
	bShouldFly = false;
}
void UMyFlyActorComponent::OriginFly()
{
	if (AActor* Owner = GetOwner())
	{
		SRPC_SetOwnerLocation(StartLocation);
		UE_LOG(LogTemp, Warning, TEXT("Moved back to start location: %s"), *StartLocation.ToString());
	}
}
void UMyFlyActorComponent::GetDelegateBool(bool delegatebool)
{
	bShouldFly = !delegatebool;

	if (bShouldFly)
	{
		APSH_BlockActor* block = Cast<APSH_BlockActor>(GetOwner());
		if(block->ActorHasTag(FName("owner")))
		block->SRPC_SetSimulatePhysics(false);
		PRINTLOG(TEXT("true"));
	}
	else
	{
		APSH_BlockActor* block = Cast<APSH_BlockActor>(GetOwner());
		if (block->ActorHasTag(FName("owner")))
		block->SRPC_SetSimulatePhysics(true);
		PRINTLOG(TEXT("fasle"));
	}
}

FPSH_FunctionBlockData UMyFlyActorComponent::SaveData()
{
	FPSH_FunctionBlockData funtionData;

	funtionData.floatArray.Add(FlySpeed);
	funtionData.floatArray.Add(FlyDistance);
	funtionData.fvectorArray.Add(FlyDirection);
	funtionData.boolArray.Add(bLoopMode);
	funtionData.boolArray.Add(bSingleDirection);
	
	return funtionData;
}

void UMyFlyActorComponent::LoadData(FPSH_FunctionBlockData funtionData)
{
	
	FlySpeed = funtionData.floatArray[0]; // float
	FlyDistance = funtionData.floatArray[1];
	FlyDirection = funtionData.fvectorArray[0]; // fvector
	bLoopMode = funtionData.boolArray[0]; // bool
	bSingleDirection = funtionData.boolArray[1]; // bool

}

void UMyFlyActorComponent::SRPC_SetOwnerLocation_Implementation(const FVector& newLocation)
{
	if (GetOwner() == nullptr) return;

	GetOwner()->SetActorLocation(newLocation);

}

void UMyFlyActorComponent::SRPC_SetOwnerSync_Implementation(FVector CFlyDirection, float CMaxFlyDistance, float CFlySpeed, bool CbLoopMode, bool CbSingleDirection)
{
	PRINTLOG(TEXT("SRPC_SetOwnerSync"));
	FlyDirection = CFlyDirection;
	FlyDistance = CMaxFlyDistance;
	FlySpeed = CFlySpeed;
	bLoopMode = CbLoopMode;
	bSingleDirection = CbSingleDirection;
}
void UMyFlyActorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMyFlyActorComponent, bLoopMode);
	DOREPLIFETIME(UMyFlyActorComponent, bShouldFly);
	DOREPLIFETIME(UMyFlyActorComponent, FlyDirection);
	DOREPLIFETIME(UMyFlyActorComponent, FlyDistance);
	DOREPLIFETIME(UMyFlyActorComponent, FlySpeed);
	DOREPLIFETIME(UMyFlyActorComponent, bSingleDirection);
}