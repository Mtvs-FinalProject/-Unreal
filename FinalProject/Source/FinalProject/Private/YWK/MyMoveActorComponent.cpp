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

	// 초기 왕복 모드
	bLoopMode = false;

	// 초기 왕복 카운트
	LoopCount = 1;
	
	// 현재 왕복 횟수
	CurrentLoop = 0;

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
		float DistanceTraveled = FVector::Dist(StartLocation, NewLocation);

		// 디버깅 로그
		UE_LOG(LogTemp, Warning, TEXT("Current Location: %s, Target Location: %s, Distance Traveled: %f, Max Distance: %f"),
			*Owner->GetActorLocation().ToString(), *NewLocation.ToString(), DistanceTraveled, MaxDistance);
		if (bSingleDirection)
		{
			// 단순 이동 모드 : 최대 거리 도달 시 이동 멈춤
			if (DistanceTraveled <= MaxDistance)
			{
				Owner->SetActorLocation(NewLocation);
				UE_LOG(LogTemp, Warning, TEXT("Actor moving to: %s"), *NewLocation.ToString());
			}
			else
			{
				StopMoving();
				UE_LOG(LogTemp, Warning, TEXT("Max distance reached. Movement stopped."));
			}
		}
		else
		{
			// 왕복 이동 모드 : 최대 거리 도달 시 방향 반전
			if (DistanceTraveled >= MaxDistance)
			{
				if (bLoopMode || CurrentLoop < LoopCount)
				{
					MoveDirection *= -1.0f; // 방향 반전
					StartLocation = Owner->GetActorLocation(); //새 출발 위치

					if (!bLoopMode)
					{
						CurrentLoop++; // 반복 횟수 증가
					}
					UE_LOG(LogTemp, Warning, TEXT("Loop %d/%d: Actor changing direction."), CurrentLoop, LoopCount);
				}
				else
				{
					StopMoving(); // 왕복 횟수 충족 시 이동 정지
					UE_LOG(LogTemp, Warning, TEXT("Movement stopped after %d loops."), CurrentLoop);
				}
			}
			Owner->SetActorLocation(NewLocation);
		}

	}
}

// 이동시작 함수
void UMyMoveActorComponent::StartMoving()
{
	if (AActor* Owner = GetOwner())
	{
		StartLocation = Owner->GetActorLocation();
	}
	CurrentLoop = 0;  // 반복 횟수 초기화
	bShouldMove = true;
	UE_LOG(LogTemp, Warning, TEXT("Movement started"));
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