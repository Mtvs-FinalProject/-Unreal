// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/MyMoveActorComponent.h"
#include "YWK/Movewidget.h"

// Sets default values for this component's properties
UMyMoveActorComponent::UMyMoveActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// 초기 이동속도
	MoveSpeed = 100.0f;

	// 초기 이동 상태
	bShouldMove = false;

	// 초기 이동방향
	MoveDirection = FVector(1.0f, 0.0f, 0.0f);

	// 이동한 거리 초기화
	MoveDistance = 0.0f;

	// 초기 최대 거리
	MoveDistance = 1000.0f;
}


// Called when the game starts
void UMyMoveActorComponent::BeginPlay()
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
void UMyMoveActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 로그로 bShouldMove 상태 확인
	if (bShouldMove)
	{
		ObjectMove(DeltaTime);
	}
}

void UMyMoveActorComponent::ObjectMove(float DeltaTime)
{
	
	if (AActor* Owner = GetOwner())
	{
		
		FVector NewLocation = Owner->GetActorLocation() + (MoveDirection * MoveSpeed * DeltaTime);
		Owner->SetActorLocation(NewLocation);

		// 이동한 거리 계산
		float DistanceTraveled = FVector::Dist(StartLocation, NewLocation);

		// 이동한 거리가 MaxDistance보다 작을 때 이동
		if (DistanceTraveled <= MaxDistance)
		{
			Owner->SetActorLocation(NewLocation);
		}
		else
		{
			StopMoving(); // 거리를 초과하면 이동 멈춤 
			UE_LOG(LogTemp, Warning, TEXT("Max distance reached. Movement stopped."));
		}
	}
}

// 이동시작 함수
void UMyMoveActorComponent::StartMoving()
{
	bShouldMove = true;
}

// 이동 끝 함수
void UMyMoveActorComponent::StopMoving()
{
	bShouldMove = false;
	UE_LOG(LogTemp, Warning, TEXT("Movement stopped!"));  // 로그 추가
}

// 시작 위치로 이동
void UMyMoveActorComponent::OriginMove()
{
	if (AActor* Owner = GetOwner())
	{
		Owner->SetActorLocation(StartLocation);
		UE_LOG(LogTemp, Warning, TEXT("Moved back to start location: %s"), *StartLocation.ToString());
	}
}

