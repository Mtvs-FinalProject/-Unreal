// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/MyRotateActorComponent.h"
#include "YWK/RotationWidget.h"
#include "Math/Quat.h"
#include "../FinalProject.h"
#include "PSH/PSH_Actor/PSH_BlockActor.h"

UMyRotateActorComponent::UMyRotateActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// �⺻ �ӵ� �� ���� ����
	RotateSpeed = 0.f;
	bShouldRot = false;
	RotateDirection = FRotator::ZeroRotator;
	MaxRotate = 360.0f;
	TotalRotationAngle = 0.0f;
	bLoopMode = false;

    SetIsReplicated(true);
}

void UMyRotateActorComponent::BeginPlay()
{
	Super::BeginPlay();

    APSH_BlockActor* block = Cast<APSH_BlockActor>(GetOwner());

    if (block)
    {
        UE_LOG(LogTemp, Warning, TEXT("Bind"));
        block->componentCreateBoolDelegate.AddUObject(this, &UMyRotateActorComponent::GetDelegateBool);
    }

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
	
    AActor* Owner = GetOwner();
    if (!Owner) return;

    if (Owner->HasAuthority()) return;

    //
    if (RotateSpeed <= 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("RotateSpeed is zero or negative for %s; skipping rotation"), *Owner->GetName());
        return;
    }

    FQuat CurrentQuat = Owner->GetActorQuat();
    FQuat RotationQuat = FQuat(RotateDirection * (RotateSpeed * DeltaTime));
    FQuat NewQuat = RotationQuat * CurrentQuat;
    NewQuat.Normalize();
    SetOwnerRotation(NewQuat);

    PRINTLOG(TEXT("Rotating %s by %f degrees"), *Owner->GetName(), RotateSpeed * DeltaTime);
}

void UMyRotateActorComponent::StartRolling()
{
     bShouldRot = true;  // ȸ�� ������ ���� �÷��� ����

     APSH_BlockActor* block = Cast<APSH_BlockActor>(GetOwner());
     if (block && block->ActorHasTag(FName("owner")))
     {
         block->SaveBlockLocations();
         PRINTLOG(TEXT("SaveBlockLocations"));
     }

     if (block)
     {
         block->SRPC_SetSimulatePhysics(false);
     }
//     TotalRotationAngle = 0.0f;
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
void UMyRotateActorComponent::GetDelegateBool(bool delegatebool)
{
    bShouldRot = delegatebool;

    if (bShouldRot)
    {
        PRINTLOG(TEXT("true"));
    }
    else
    {
        PRINTLOG(TEXT("fasle"));
    }
}
FPSH_FunctionBlockData UMyRotateActorComponent::SaveData()
{
    FPSH_FunctionBlockData funtionData;
    PRINTLOG(TEXT("UMyRotateActorComponent::SaveData()"));
  
    funtionData.floatArray.Add(RotateSpeed);
    funtionData.intArray.Add(RotateNum);
    funtionData.frotatorArray.Add(RotateDirection);
    funtionData.boolArray.Add(bLoopMode);

    return funtionData;
}
void UMyRotateActorComponent::LoadData(FPSH_FunctionBlockData funtionData)
{
    RotateSpeed = funtionData.floatArray[0]; //float
    RotateNum = funtionData.intArray[0];//(ȸ�� Ƚ��)->int32
    RotateDirection = funtionData.frotatorArray[0]; //FRotator
    bLoopMode = funtionData.boolArray[0];//bool
}

void UMyRotateActorComponent::SetOwnerRotation_Implementation(const FQuat& newRotation)
{
    if (GetOwner() == nullptr) return;

    GetOwner()->SetActorRotation(newRotation);

    PRINTLOG(TEXT("SetOwnerLocation_Implementation"));
}