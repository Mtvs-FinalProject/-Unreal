// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/MyRotateActorComponent.h"
#include "YWK/RotationWidget.h"
#include "Math/Quat.h"

UMyRotateActorComponent::UMyRotateActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// �⺻ �ӵ� �� ���� ����
	RotateSpeed = 100.0f;
	bShouldRot = false;
	RotateDirection = FRotator(1.0f, 0.0f, 0.0f);
	MaxRotate = 360.0f;
	TotalRotationAngle = 0.0f;
	bLoopMode = false;
}

void UMyRotateActorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		StartRotation = Owner->GetActorRotation();
		UE_LOG(LogTemp, Warning, TEXT("Initial Rotation stored : %s"), *StartRotation.ToString());
	}
}

void UMyRotateActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bShouldRot)
    {
        UE_LOG(LogTemp, Warning, TEXT("bShouldRot is true for %s; rotation will proceed"), *GetOwner()->GetName());

        // ���� �ð��� ������
        float CurrentTime = GetWorld()->GetTimeSeconds();

        // ������ �α� ��� �� 2�ʰ� �����ٸ� �α� ���
        if (CurrentTime - LastLogTime >= 2.0f)
        {
            UE_LOG(LogTemp, Warning, TEXT("Rotating component %s with speed: %f and direction: %s"),
                *GetOwner()->GetName(),
                RotateSpeed,
                *RotateDirection.ToString());

            // ������ �α� ��� �ð��� ����
            LastLogTime = CurrentTime;
        }

        ObjectRolling(DeltaTime);  // ȸ�� ����
    }
    else
    {
        //UE_LOG(LogTemp, Warning, TEXT("bShouldRot is false for %s; rotation skipped"), *GetOwner()->GetName());
    }
}

void UMyRotateActorComponent::ObjectRolling(float DeltaTime)
{
    if (AActor* Owner = GetOwner())
    {
        if (RotateSpeed <= 0.0f)
        {
            UE_LOG(LogTemp, Warning, TEXT("RotateSpeed is zero or negative for %s; skipping rotation"), *Owner->GetName());
            return;
        }

        FQuat CurrentQuat = Owner->GetActorQuat();
        FQuat RotationQuat = FQuat(RotateDirection * (RotateSpeed * DeltaTime));
        FQuat NewQuat = RotationQuat * CurrentQuat;
        NewQuat.Normalize();
        Owner->SetActorRotation(NewQuat);

        UE_LOG(LogTemp, Warning, TEXT("Rotating %s by %f degrees"), *Owner->GetName(), RotateSpeed * DeltaTime);
    }
}

void UMyRotateActorComponent::StartRolling()
{
    bShouldRot = true;  // ȸ�� ������ ���� �÷��� ����
    TotalRotationAngle = 0.0f;
    UE_LOG(LogTemp, Warning, TEXT("StartRolling called and bShouldRot set to true for %s"), *GetOwner()->GetName());
}

void UMyRotateActorComponent::StopRolling()
{
    bShouldRot = false;
    TotalRotationAngle = 0.0f;  // ȸ�� �ʱ�ȭ
    UE_LOG(LogTemp, Warning, TEXT("Rolling stopped!"));
}

void UMyRotateActorComponent::OriginRolling()
{
	if (AActor* Owner = GetOwner())
	{
		Owner->SetActorRotation(StartRotation);
		TotalRotationAngle = 0.0f;  // ���� ���� �ʱ�ȭ
		UE_LOG(LogTemp, Warning, TEXT("Moved back to start Rotation: %s"), *StartRotation.ToString());
	}
}
