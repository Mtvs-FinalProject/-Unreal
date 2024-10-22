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

	// �ʱ� ȸ���ӵ�
	RotateSpeed = 100;

	// �ʱ� ȸ�� ����
	bShouldRot = false;

	// �ʱ� ȸ�� ����
	RotateDirection = FRotator(1.0f, 0.0f, 0.0f);

	// ȸ�� Ƚ�� �ʱ�ȭ
	RotateNum = 0;

	// �ִ� ȸ�� Ƚ��
	MaxRotate = 100;
}


// Called when the game starts
void UMyRotateActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// �ʱ� ȸ�� ���� ����
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
		// ���� ȸ�� �� ��������
		FQuat CurrentQuat = Owner->GetActorQuat();

		// ȸ�� ������ ������� ��ȯ (FRotator ��� FQuat ���)
		FQuat RotationQuat = FQuat(RotateDirection * RotateSpeed * DeltaTime);

		// ���ο� ȸ�� �� ��� (������� �������� ȸ��)
		FQuat NewQuat = RotationQuat * CurrentQuat;
		NewQuat.Normalize();  // ������� ����ȭ�� ������ ����

		// ȸ�� ����
		Owner->SetActorRotation(NewQuat.Rotator());

		// ȸ���� ���� ���� (Yaw, Pitch, Roll ��� ��ü ȸ�� ����)
		float RotateAngle = StartRotation.Quaternion().AngularDistance(NewQuat) * (180.0f / PI);

		// 360�� ȸ���� ���� �ʵ��� ���� ����
		if (RotateAngle >= 360.0f)
		{
			StopRolling();
			UE_LOG(LogTemp, Warning, TEXT("360-degree rotation completed. Rolling stopped."));
		}
	}
}


// ȸ�� ����
void UMyRotateActorComponent::StartRolling()
{
	bShouldRot = true;
}
// ȸ�� ����
void UMyRotateActorComponent::StopRolling()
{
	bShouldRot = false;
	UE_LOG(LogTemp, Warning, TEXT("Rolling stopped!"));
}

// ȸ�� ����ġ
void UMyRotateActorComponent::OriginRolling()
{
	if (AActor* Owner = GetOwner())
	{
		Owner->SetActorRotation(StartRotation);
		UE_LOG(LogTemp, Warning, TEXT("Moved back to start Rotation: %s"), *StartRotation.ToString());
	}
}

