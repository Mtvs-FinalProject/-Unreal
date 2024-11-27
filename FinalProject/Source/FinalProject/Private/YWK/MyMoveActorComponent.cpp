// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/MyMoveActorComponent.h"
#include "YWK/Movewidget.h"
#include "PSH/PSH_Actor/PSH_BlockActor.h"
#include "../FinalProject.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UMyMoveActorComponent::UMyMoveActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// 초기 이동속도
	MoveSpeed = 0.f;

	// 초기 이동 상태
	bShouldMove = false;

	// 초기 이동방향
	MoveDirection = FVector::ZeroVector;

	// 이동한 거리 초기화
	MoveDistance = 0.0f;

	// 초기 최대 거리
    MaxDistance = 0.f;

	bSingleDirection = true;

	// 초기 왕복 모드
	bLoopMode = false;
	
	// 현재 왕복 횟수
	CurrentLoop = 0;

    SetIsReplicated(true);

}


// Called when the game starts
void UMyMoveActorComponent::BeginPlay()
{
	Super::BeginPlay();

    APSH_BlockActor* block = Cast<APSH_BlockActor>(GetOwner());

    if (block)
    {
        block->componentCreateBoolDelegate.AddUObject(this, &UMyMoveActorComponent::GetDelegateBool);
    }
	StartLocation = GetOwner()->GetActorLocation();
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

   // if(Owner->HasAuthority()) return;

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
        SRPC_SetOwnerLocation(NewLocation);
    }
}

// 이동시작 함수
void UMyMoveActorComponent::StartMoving()
{
    StartLocation = GetOwner()->GetActorLocation();

   SRPC_SetOwnerSync(MoveDirection,MaxDistance,MoveSpeed,bLoopMode,bSingleDirection);

   APSH_BlockActor* block = Cast<APSH_BlockActor>(GetOwner());
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
    SRPC_StopMoving();
	
    APSH_BlockActor* block = Cast<APSH_BlockActor>(GetOwner());

    if (block)
    {
        block->SRPC_SetSimulatePhysics(false);
    }
	
}

void UMyMoveActorComponent::SRPC_StopMoving_Implementation()
{
    MRPC_StopMoving();
}
void UMyMoveActorComponent::MRPC_StopMoving_Implementation()
{
    bShouldMove = false;
    PRINTLOG(TEXT("Movement stopped!"));  // 로그 추가
}
// 시작 위치로 이동
void UMyMoveActorComponent::OriginMove()
{
	if (AActor* Owner = GetOwner())
	{
        SRPC_SetOwnerLocation(StartLocation);
	}
}

void UMyMoveActorComponent::GetDelegateBool(bool delegatebool)
{   
    bShouldMove = !delegatebool;


    APSH_BlockActor* block = Cast<APSH_BlockActor>(GetOwner());
    if (block->ActorHasTag(FName("owner")))
        block->SRPC_SetSimulatePhysics(false);
//     if (bShouldMove)
//     {
//         APSH_BlockActor* block = Cast<APSH_BlockActor>(GetOwner());
//         if (block->ActorHasTag(FName("owner")))
//             block->SRPC_SetSimulatePhysics(false);
//         PRINTLOG(TEXT("true"));
//     }
//     else
//     {
//         APSH_BlockActor* block = Cast<APSH_BlockActor>(GetOwner());
//         if (block->ActorHasTag(FName("owner")))
//             block->SRPC_SetSimulatePhysics(true);
//         PRINTLOG(TEXT("fasle"));
//     }
}
FPSH_FunctionBlockData UMyMoveActorComponent::SaveData()
{
        FPSH_FunctionBlockData funtionData;

	    funtionData.floatArray.Add(MoveSpeed);
	    funtionData.floatArray.Add(MaxDistance);
		funtionData.fvectorArray.Add(MoveDirection);
	    funtionData.boolArray.Add(bLoopMode);
	    funtionData.boolArray.Add(bSingleDirection);

        return funtionData;
}
void UMyMoveActorComponent::LoadData(FPSH_FunctionBlockData funtionData)
{
        MoveSpeed = funtionData.floatArray[0]; // float
        MaxDistance = funtionData.floatArray[1]; // float
        MoveDirection = funtionData.fvectorArray[0]; //FVector
        bLoopMode = funtionData.boolArray[0]; //bool
        bSingleDirection = funtionData.boolArray[1]; //bool

}

void UMyMoveActorComponent::SRPC_SetOwnerLocation_Implementation(const FVector& newLocation)
{
    if (GetOwner() == nullptr) return;

    GetOwner()->SetActorLocation(newLocation);
}

void UMyMoveActorComponent::SRPC_SetOwnerSync_Implementation(FVector CMoveDirection, float CMaxDistance, float CMoveSpeed, bool CbLoopMode ,bool CbSingleDirection)
{
    MoveSpeed = CMoveSpeed; // float
    MaxDistance = CMaxDistance; // float
    MoveDirection = CMoveDirection;
    bLoopMode = CbLoopMode; //bool
    bSingleDirection = CbSingleDirection;
}

void UMyMoveActorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UMyMoveActorComponent, MoveSpeed);
    DOREPLIFETIME(UMyMoveActorComponent, MaxDistance);
    DOREPLIFETIME(UMyMoveActorComponent, MoveDirection);
    DOREPLIFETIME(UMyMoveActorComponent, bLoopMode);
    DOREPLIFETIME(UMyMoveActorComponent, bSingleDirection);
    DOREPLIFETIME(UMyMoveActorComponent, bShouldMove);
}