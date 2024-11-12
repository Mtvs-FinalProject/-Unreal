// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/MyRotateActorComponent.h"
#include "YWK/RotationWidget.h"
#include "Math/Quat.h"

UMyRotateActorComponent::UMyRotateActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// 기본 속도 및 상태 설정
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

        // 현재 시간을 가져옴
        float CurrentTime = GetWorld()->GetTimeSeconds();

        // 마지막 로그 출력 후 2초가 지났다면 로그 출력
        if (CurrentTime - LastLogTime >= 2.0f)
        {
            UE_LOG(LogTemp, Warning, TEXT("Rotating component %s with speed: %f and direction: %s"),
                *GetOwner()->GetName(),
                RotateSpeed,
                *RotateDirection.ToString());

            // 마지막 로그 출력 시간을 갱신
            LastLogTime = CurrentTime;
        }

        ObjectRolling(DeltaTime);  // 회전 수행
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
    bShouldRot = true;  // 회전 시작을 위한 플래그 설정
    TotalRotationAngle = 0.0f;
    UE_LOG(LogTemp, Warning, TEXT("StartRolling called and bShouldRot set to true for %s"), *GetOwner()->GetName());
}

void UMyRotateActorComponent::StopRolling()
{
    bShouldRot = false;
    TotalRotationAngle = 0.0f;  // 회전 초기화
    UE_LOG(LogTemp, Warning, TEXT("Rolling stopped!"));
}

void UMyRotateActorComponent::OriginRolling()
{
	if (AActor* Owner = GetOwner())
	{
		Owner->SetActorRotation(StartRotation);
		TotalRotationAngle = 0.0f;  // 누적 각도 초기화
		UE_LOG(LogTemp, Warning, TEXT("Moved back to start Rotation: %s"), *StartRotation.ToString());
	}
}
