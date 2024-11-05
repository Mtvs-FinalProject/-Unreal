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

	// �ʱ� ��ġ ����
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

	// �α׷� bShouldMove ���� Ȯ��
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

		// ����� �α�
		UE_LOG(LogTemp, Warning, TEXT("Current Location: %s, Target Location: %s, Distance Traveled: %f, Max Distance: %f"),
			*Owner->GetActorLocation().ToString(), *NewLocation.ToString(), DistanceTraveled, MaxDistance);
		if (bSingleDirection)
		{
			// �ܼ� �̵� ��� : �ִ� �Ÿ� ���� �� �̵� ����
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
			// �պ� �̵� ��� : �ִ� �Ÿ� ���� �� ���� ����
			if (DistanceTraveled >= MaxDistance)
			{
				if (bLoopMode || CurrentLoop < LoopCount)
				{
					MoveDirection *= -1.0f; // ���� ����
					StartLocation = Owner->GetActorLocation(); //�� ��� ��ġ

					if (!bLoopMode)
					{
						CurrentLoop++; // �ݺ� Ƚ�� ����
					}
					UE_LOG(LogTemp, Warning, TEXT("Loop %d/%d: Actor changing direction."), CurrentLoop, LoopCount);
				}
				else
				{
					StopMoving(); // �պ� Ƚ�� ���� �� �̵� ����
					UE_LOG(LogTemp, Warning, TEXT("Movement stopped after %d loops."), CurrentLoop);
				}
			}
			Owner->SetActorLocation(NewLocation);
		}

	}
}

// �̵����� �Լ�
void UMyMoveActorComponent::StartMoving()
{
	if (AActor* Owner = GetOwner())
	{
		StartLocation = Owner->GetActorLocation();
	}
	CurrentLoop = 0;  // �ݺ� Ƚ�� �ʱ�ȭ
	bShouldMove = true;
	UE_LOG(LogTemp, Warning, TEXT("Movement started"));
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