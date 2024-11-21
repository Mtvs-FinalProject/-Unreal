// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/MyMoveActorComponent.h"
#include "YWK/Movewidget.h"
#include "PSH/PSH_Actor/PSH_BlockActor.h"
#include "../FinalProject.h"

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

	bSingleDirection = true;

	// 초기 왕복 모드
	bLoopMode = false;

	// 초기 왕복 카운트
	LoopCount = 1;
	
	// 현재 왕복 횟수
	CurrentLoop = 0;

    SetIsReplicated(true);
}


// Called when the game starts
void UMyMoveActorComponent::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetOwner()->GetActorLocation();
	UE_LOG(LogTemp, Warning, TEXT("Initial Location stored : %s"), *StartLocation.ToString());
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
    AActor* Owner = GetOwner();
    if (!Owner) return;

    FVector NewLocation = Owner->GetActorLocation() + (MoveDirection * MoveSpeed * DeltaTime);
    float DistanceTraveled = FVector::Dist(StartLocation, NewLocation);

    if (DistanceTraveled >= MaxDistance)
    {
        if (bSingleDirection)
        {
            StopMoving();
        }
        else
        {
            // Loop mode: reverse direction and reset start location
            if (bLoopMode)
            {
                // Reverse direction
                MoveDirection *= -1.0f;
                StartLocation = Owner->GetActorLocation();

            }
            else
            {
                // Stop moving if loop count limit has been reached
                StopMoving();
            }
        }
    }
    else
    {

        SetOwnerLocation(NewLocation);
        // Move actor to new location
      //  Owner->SetActorLocation(NewLocation);
    }
}

// 이동시작 함수
void UMyMoveActorComponent::StartMoving()
{
    StartLocation = GetOwner()->GetActorLocation();

    APSH_BlockActor * block = Cast<APSH_BlockActor>(GetOwner());
    if (block && block->ActorHasTag(FName("owner")))
    {
        block->SaveBlockLocations();
        PRINTLOG(TEXT("SaveBlockLocations"));
    }
    bShouldMove = true;

    UE_LOG(LogTemp, Warning, TEXT("Movement started with direction: %s"), *MoveDirection.ToString());
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

void UMyMoveActorComponent::GetDelegateBool(bool delegatebool)
{   
    bShouldMove = delegatebool;

    if (bShouldMove)
    {
        UE_LOG(LogTemp, Warning, TEXT("true"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("fasle"));
    }
}
FPSH_FunctionBlockData UMyMoveActorComponent::SaveData()
{
        FPSH_FunctionBlockData funtionData;
	    funtionData.floatArray.Add(MoveSpeed);
	    funtionData.floatArray.Add(MoveDistance);
	    funtionData.fvectorArray.Add(MoveDirection);
	    funtionData.intArray.Add(LoopCount);
	    funtionData.boolArray.Add(bLoopMode);

//         MoveSpeed; // float
// 		MoveDistance; // float
//         MoveDirection; //FVector
//         LoopCount; //int32
//         bLoopMode; //bool

        return funtionData;
}
void UMyMoveActorComponent::LoadData(FPSH_FunctionBlockData funtionData)
{
        MoveSpeed = funtionData.floatArray[0]; // float
        MaxDistance = funtionData.floatArray[1]; // float
        MoveDirection = funtionData.fvectorArray[0]; //FVector
        LoopCount = funtionData.intArray[0]; //int32
        bLoopMode = funtionData.boolArray[0]; //bool
        bShouldMove = true;
        PRINTLOG(TEXT("MoveSpeed: %f"), MoveSpeed);
        PRINTLOG(TEXT("MaxDistance: %f"), MaxDistance);
        PRINTLOG(TEXT("MoveDirection: %s"), *MoveDirection.ToString());
        PRINTLOG(TEXT("LoopCount: %d"), LoopCount);
        PRINTLOG(TEXT("bLoopMode: %s"), bLoopMode ? TEXT("True") : TEXT("False"));
}
void UMyMoveActorComponent::SetOwnerLocation_Implementation(const FVector& newLocation)
{
    if(GetOwner() == nullptr) return;

    GetOwner()->SetActorLocation(newLocation);
}