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

	// �ʱ� �̵��ӵ�
	MoveSpeed = 0.f;

	// �ʱ� �̵� ����
	bShouldMove = false;

	// �ʱ� �̵�����
	MoveDirection = FVector::ZeroVector;

	// �̵��� �Ÿ� �ʱ�ȭ
	MoveDistance = 0.0f;

	// �ʱ� �ִ� �Ÿ�
    MaxDistance = 0.f;

	bSingleDirection = true;

	// �ʱ� �պ� ���
	bLoopMode = false;
	
	// ���� �պ� Ƚ��
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
        UE_LOG(LogTemp, Warning, TEXT("Bind"));
        block->componentCreateBoolDelegate.AddUObject(this, &UMyMoveActorComponent::GetDelegateBool);
    }

	StartLocation = GetOwner()->GetActorLocation();
	UE_LOG(LogTemp, Warning, TEXT("Initial Location stored : %s"), *StartLocation.ToString());
}


// Called every frame
void UMyMoveActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// �α׷� bShouldMove ���� Ȯ��
	if (bShouldMove)
	{
		ObjectMove(DeltaTime);
	}
}

void UMyMoveActorComponent::ObjectMove(float DeltaTime)
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    if(Owner->HasAuthority()) return;

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
        // Move actor to new location
      //  Owner->SetActorLocation(NewLocation);
    }
}

// �̵����� �Լ�
void UMyMoveActorComponent::StartMoving()
{
    StartLocation = GetOwner()->GetActorLocation();

   


    bShouldMove = true;

    UE_LOG(LogTemp, Warning, TEXT("Movement started with direction: %s"), *MoveDirection.ToString());
}

// �̵� �� �Լ�
void UMyMoveActorComponent::StopMoving()
{
	bShouldMove = false;
	UE_LOG(LogTemp, Warning, TEXT("Movement stopped!"));  // �α� �߰�
}

// ���� ��ġ�� �̵�
void UMyMoveActorComponent::OriginMove()
{
	if (AActor* Owner = GetOwner())
	{
        SRPC_SetOwnerLocation(StartLocation);
		UE_LOG(LogTemp, Warning, TEXT("Moved back to start location: %s"), *StartLocation.ToString());
	}
}

void UMyMoveActorComponent::GetDelegateBool(bool delegatebool)
{   
    bShouldMove = delegatebool;

    if (bShouldMove)
    {
        PRINTLOG(TEXT("true"));
    }
    else
    {
        PRINTLOG(TEXT("fasle"));
    }
}
FPSH_FunctionBlockData UMyMoveActorComponent::SaveData()
{
        FPSH_FunctionBlockData funtionData;
	    funtionData.floatArray.Add(MoveSpeed);
	    funtionData.floatArray.Add(MaxDistance);
	    funtionData.fvectorArray.Add(MoveDirection);
	    funtionData.intArray.Add(LoopCount);
	    funtionData.boolArray.Add(bLoopMode);
	    funtionData.boolArray.Add(bSingleDirection);

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
        bSingleDirection = funtionData.boolArray[1]; //bool
  
        PRINTLOG(TEXT("MoveSpeed: %f"), MoveSpeed);
        PRINTLOG(TEXT("MaxDistance: %f"), MaxDistance);
        PRINTLOG(TEXT("MoveDirection: %s"), *MoveDirection.ToString());
        PRINTLOG(TEXT("LoopCount: %d"), LoopCount);
        PRINTLOG(TEXT("bLoopMode: %s"), bLoopMode ? TEXT("True") : TEXT("False"));
       
}

void UMyMoveActorComponent::SRPC_SetOwnerLocation_Implementation(const FVector& newLocation)
{
    if (GetOwner() == nullptr) return;

    GetOwner()->SetActorLocation(newLocation);
}
