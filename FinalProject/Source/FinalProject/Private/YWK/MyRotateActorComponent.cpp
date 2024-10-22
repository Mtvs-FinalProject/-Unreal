// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/MyRotateActorComponent.h"
#include "YWK/RotationWidget.h"
#include "Math/Quat.h"

// Sets default values for this component's properties
UMyRotateActorComponent::UMyRotateActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// 초기 회전속도
	RotateSpeed = 100;

	// 초기 회전 상태
	bShouldRot = false;

	// 초기 회전 방향
	RotateDirection = FRotator(1.0f, 0.0f, 0.0f);

	// 회전 횟수 초기화
	RotateNum = 0;

	// 최대 회전 횟수
	MaxRotate = 100;
}


// Called when the game starts
void UMyRotateActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// 초기 회전 방향 설정
	if (AActor* Owner = GetOwner())
	{
		StartRotation = Owner->GetActorRotation();
		UE_LOG(LogTemp, Warning, TEXT("Initial Rotation stored : %s"), *StartRotation.ToString());
	}

}


// Called every frame
void UMyRotateActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bShouldRot)
	{
		ObjectRolling(DeltaTime);
	}
	
}

void UMyRotateActorComponent::ObjectRolling(float DeltaTime)
{
	if (AActor* Owner = GetOwner())
	{
		// 현재 회전 값 가져오기
		FQuat CurrentQuat = Owner->GetActorQuat();

		// 회전 방향을 사원수로 변환 (FRotator 대신 FQuat 사용)
		FQuat RotationQuat = FQuat(RotateDirection * RotateSpeed * DeltaTime);

		// 새로운 회전 값 계산 (사원수의 곱셈으로 회전)
		FQuat NewQuat = RotationQuat * CurrentQuat;
		NewQuat.Normalize();  // 사원수의 정규화로 안정성 유지

		// 회전 설정
		Owner->SetActorRotation(NewQuat.Rotator());

		// 회전한 각도 누적 (Yaw, Pitch, Roll 대신 전체 회전 각도)
		float RotateAngle = StartRotation.Quaternion().AngularDistance(NewQuat) * (180.0f / PI);

		// 360도 회전을 넘지 않도록 각도 제한
		if (RotateAngle >= 360.0f)
		{
			StopRolling();
			UE_LOG(LogTemp, Warning, TEXT("360-degree rotation completed. Rolling stopped."));
		}
	}
}


// 회전 시작
void UMyRotateActorComponent::StartRolling()
{
	bShouldRot = true;
}
// 회전 멈춤
void UMyRotateActorComponent::StopRolling()
{
	bShouldRot = false;
	UE_LOG(LogTemp, Warning, TEXT("Rolling stopped!"));
}

// 회전 원위치
void UMyRotateActorComponent::OriginRolling()
{
	if (AActor* Owner = GetOwner())
	{
		Owner->SetActorRotation(StartRotation);
		UE_LOG(LogTemp, Warning, TEXT("Moved back to start Rotation: %s"), *StartRotation.ToString());
	}
}

