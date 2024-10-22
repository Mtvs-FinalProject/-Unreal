// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/MyFlyActorComponent.h"

// Sets default values for this component's properties
UMyFlyActorComponent::UMyFlyActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// �ʱ� ��� �ӵ�
	FlySpeed = 100;

	// �ʱ� ��� ����
	bShouldFly = false;

	// �ʱ� ��� ����
	FlyDirection = FVector(0.0f, 0.0f, 1.0f);

	// ����� ���� �ʱ�ȭ
	FlyDistance = 0.0f;

	// �ʱ� ��� �ִ� ����
	MaxFlyDistance = 1000.0f;
}


// Called when the game starts
void UMyFlyActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// �ʱ� ��ġ ����
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

	// �α׷� bShouldFly ���� Ȯ��
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

		// �̵��� �Ÿ� ���
		float DistanceTraveled = FVector::Dist(StartLocation, NewLocation);

		// �̵��� �Ÿ��� MaxDistance���� ���� ���� �̵�
		if (DistanceTraveled <= MaxFlyDistance)
		{
			Owner->SetActorLocation(NewLocation);
		}
		else
		{
			StopFly(); // �Ÿ��� �ʰ��ϸ� �̵� ����
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

