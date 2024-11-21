// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/MyFlyActorComponent.h"
#include "PSH/PSH_Actor/PSH_BlockActor.h"
#include "../FinalProject.h"

// Sets default values for this component's properties
UMyFlyActorComponent::UMyFlyActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// 초기 상승 속도
	FlySpeed = 100;

	// 초기 상승 상태
	bShouldFly = false;
	
	// 초기 상승 방향
	FlyDirection = FVector(0.0f, 0.0f, 1.0f);

	// 상승한 높이 초기화
	FlyDistance = 0.0f;

	// 초기 상승 최대 높이
	MaxFlyDistance = 1000.0f;

	bLoopMode = false;
	bSingleDirection = false;
	LoopCount = 1;
	CurrentLoop = 0;
}

// Called when the game starts
void UMyFlyActorComponent::BeginPlay()
{
	Super::BeginPlay();

	APSH_BlockActor* block = Cast<APSH_BlockActor>(GetOwner());

	if (block)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bind"));
		block->componentCreateBoolDelegate.AddUObject(this,&UMyFlyActorComponent::GetDelegateBool);
	}

	// 초기 위치 저장
    StartLocation = GetOwner()->GetActorLocation();
    UE_LOG(LogTemp, Warning, TEXT("Initial Location stored : %s"), *StartLocation.ToString());
}


// Called every frame
void UMyFlyActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 로그로 bShouldFly 상태 확인
	//UE_LOG(LogTemp, Warning, TEXT("TickComponent: bShouldFly = %s"), bShouldFly ? TEXT("true") : TEXT("false"));

	if (bShouldFly)
	{
		ObjectFly(DeltaTime);
	}
}

void UMyFlyActorComponent::ObjectFly(float DeltaTime)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	FVector NewLocation = Owner->GetActorLocation() + (FlyDirection * FlySpeed * DeltaTime);
	float DistanceTraveled = FVector::Dist(StartLocation, NewLocation);

	float Tolerance = 1.0f;

	if (bSingleDirection)
	{
		if (DistanceTraveled <= MaxFlyDistance)
		{
			Owner->SetActorLocation(NewLocation);
		}
		else
		{
			StopFly();
			UE_LOG(LogTemp, Warning, TEXT("Reached max distance in single direction. Stopping flight."));
		}
	}
	else
	{
		if (DistanceTraveled >= MaxFlyDistance - Tolerance)
		{
			if (bLoopMode || CurrentLoop < LoopCount)
			{
				FlyDirection *= -1.0f;
				StartLocation = Owner->GetActorLocation();

				if (!bLoopMode)
				{
					CurrentLoop++;
				}
				UE_LOG(LogTemp, Warning, TEXT("Loop %d/%d: Reversing direction for fly. New direction: %s"), CurrentLoop, LoopCount, *FlyDirection.ToString());
			}
			else
			{
				StopFly();
				UE_LOG(LogTemp, Warning, TEXT("Max loop count reached. Stopping flight."));
			}
		}
		else
		{
			Owner->SetActorLocation(NewLocation);
		}
	}
}


void UMyFlyActorComponent::StartFly()
{
	StartLocation = GetOwner()->GetActorLocation();
	CurrentLoop = 0;
	bShouldFly = true;
	UE_LOG(LogTemp, Warning, TEXT("Flight started with direction: %s"), *FlyDirection.ToString());
}

void UMyFlyActorComponent::StopFly()
{
	bShouldFly = false;
	UE_LOG(LogTemp, Warning, TEXT("StopFly called. bShouldFly = %s"), bShouldFly ? TEXT("true") : TEXT("false"));
}

void UMyFlyActorComponent::OriginFly()
{
	if (AActor* Owner = GetOwner())
	{
		Owner->SetActorLocation(StartLocation);
		UE_LOG(LogTemp, Warning, TEXT("Moved back to start location: %s"), *StartLocation.ToString());
	}
}
void UMyFlyActorComponent::GetDelegateBool(bool delegatebool)
{
	bShouldFly = delegatebool;

	if (bShouldFly)
	{
		UE_LOG(LogTemp,Warning,TEXT("true"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("fasle"));
	}
}

FPSH_FunctionBlockData UMyFlyActorComponent::SaveData()
{
	FPSH_FunctionBlockData funtionData;

	funtionData.floatArray.Add(FlySpeed);
	funtionData.fvectorArray.Add(FlyDirection);
	funtionData.intArray.Add(LoopCount);
	funtionData.boolArray.Add(bLoopMode);
	
// 	FlySpeed; // float
// 	FlyDirection; // fvector
// 	LoopCount;	// int32
// 	bLoopMode; // bool

	return funtionData;
}
void UMyFlyActorComponent::LoadData(FPSH_FunctionBlockData funtionData)
{
	
	FlySpeed = funtionData.floatArray[0]; // float
	FlyDirection = funtionData.fvectorArray[0]; // fvector
	LoopCount = funtionData.intArray[0];	// int32
	bLoopMode = funtionData.boolArray[0]; // bool

// 	UE_LOG(LogTemp, Log, TEXT("FlySpeed: %f"), FlySpeed);
// 	UE_LOG(LogTemp, Log, TEXT("FlyDirection: %s"), *FlyDirection.ToString());
// 	UE_LOG(LogTemp, Log, TEXT("LoopCount: %d"), LoopCount);
// 	UE_LOG(LogTemp, Log, TEXT("bLoopMode: %s"), bLoopMode ? TEXT("True") : TEXT("False"));
}