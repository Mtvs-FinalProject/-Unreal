// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/MyFlyActorComponent.h"

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
}


// Called when the game starts
void UMyFlyActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// 초기 위치 저장
	if (AActor* Owner = GetOwner())
	{
		StartLocation = Owner->GetActorLocation();
		UE_LOG(LogTemp, Warning, TEXT("Initial Location stored : %s"), *StartLocation.ToString());
	}	
}


// Called every frame
void UMyFlyActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 로그로 bShouldFly 상태 확인
	UE_LOG(LogTemp, Warning, TEXT("TickComponent: bShouldFly = %s"), bShouldFly ? TEXT("true") : TEXT("false"));

	if (bShouldFly)
	{
		ObjectFly(DeltaTime);
	}
}

void UMyFlyActorComponent::ObjectFly(float DeltaTime)
{
	if (AActor* Owner = GetOwner())
	{
		FVector NewLocation = Owner->GetActorLocation() + (FlyDirection * FlySpeed * DeltaTime);
		Owner->SetActorLocation(NewLocation);

		// 이동한 거리 계산
		float DistanceTraveled = FVector::Dist(StartLocation, NewLocation);

		// 이동한 거리가 MaxDistance보다 작을 때만 이동
		if (DistanceTraveled <= MaxFlyDistance)
		{
			Owner->SetActorLocation(NewLocation);
		}
		else
		{
			StopFly(); // 거리를 초과하면 이동 멈춤
			UE_LOG(LogTemp, Warning, TEXT("Max distance reached. Movement stopped."));
		}
	}
}

void UMyFlyActorComponent::StartFly()
{
	bShouldFly = true;
	UE_LOG(LogTemp, Warning, TEXT("StartFly called. bShouldFly = %s"), bShouldFly ? TEXT("true") : TEXT("false"));
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

