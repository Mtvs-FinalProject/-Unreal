// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/MyMoveActorComponent.h"
#include "YWK/Movewidget.h"

// Sets default values for this component's properties
UMyMoveActorComponent::UMyMoveActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// �ʱ� �̵��ӵ�
	MoveSpeed = 100.0f;

	// �ʱ� �̵� ����
	bShouldMove = false;

	// �ʱ� �̵�����
	MoveDirection = FVector(1.0f, 0.0f, 0.0f);

	// �̵��� �Ÿ� �ʱ�ȭ
	MoveDistance = 0.0f;

	// �ʱ� �ִ� �Ÿ�
	MoveDistance = 1000.0f;

	bSingleDirection = true;

	// �ʱ� �պ� ���
	bLoopMode = false;

	// �ʱ� �պ� ī��Ʈ
	LoopCount = 1;
	
	// ���� �պ� Ƚ��
	CurrentLoop = 0;

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

    FVector NewLocation = Owner->GetActorLocation() + (MoveDirection * MoveSpeed * DeltaTime);
    float DistanceTraveled = FVector::Dist(StartLocation, NewLocation);

    UE_LOG(LogTemp, Warning, TEXT("Current Location: %s, Distance Traveled: %f, Max Distance: %f"),
        *NewLocation.ToString(), DistanceTraveled, MaxDistance);

    if (DistanceTraveled >= MaxDistance)
    {
        if (bSingleDirection)
        {
            // If in single direction mode, stop movement once max distance is reached
            StopMoving();
            UE_LOG(LogTemp, Warning, TEXT("Reached max distance in single direction. Stopping movement."));
        }
        else
        {
            // Loop mode: reverse direction and reset start location
            if (bLoopMode || CurrentLoop < LoopCount)
            {
                // Reverse direction
                MoveDirection *= -1.0f;
                StartLocation = Owner->GetActorLocation();

                // Increment loop count if not in continuous loop mode
                if (!bLoopMode)
                {
                    CurrentLoop++;
                }
                UE_LOG(LogTemp, Warning, TEXT("Loop %d/%d: Reversing direction. New direction: %s"),
                    CurrentLoop, LoopCount, *MoveDirection.ToString());
            }
            else
            {
                // Stop moving if loop count limit has been reached
                StopMoving();
                UE_LOG(LogTemp, Warning, TEXT("Max loop count reached. Stopping movement."));
            }
        }
    }
    else
    {
        // Move actor to new location
        Owner->SetActorLocation(NewLocation);
        UE_LOG(LogTemp, Warning, TEXT("Actor moving to: %s"), *NewLocation.ToString());
    }
}

// �̵����� �Լ�
void UMyMoveActorComponent::StartMoving()
{
    StartLocation = GetOwner()->GetActorLocation();
    CurrentLoop = 0;
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
		Owner->SetActorLocation(StartLocation);
		UE_LOG(LogTemp, Warning, TEXT("Moved back to start location: %s"), *StartLocation.ToString());
	}
}