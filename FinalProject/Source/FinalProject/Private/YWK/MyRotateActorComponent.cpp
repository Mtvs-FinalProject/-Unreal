// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/MyRotateActorComponent.h"
#include "YWK/RotationWidget.h"
#include "Math/Quat.h"
#include "../FinalProject.h"
#include "PSH/PSH_Actor/PSH_BlockActor.h"
#include "Net/UnrealNetwork.h"

UMyRotateActorComponent::UMyRotateActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// 기본 속도 및 상태 설정
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
        block->componentCreateBoolDelegate.AddUObject(this, &UMyRotateActorComponent::GetDelegateBool);
    }

	if (AActor* Owner = GetOwner())
	{
		StartRotation = Owner->GetActorRotation();
	}
}

void UMyRotateActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    

    if (bShouldRot)
    {
      //  UE_LOG(LogTemp, Warning, TEXT("bShouldRot is true for %s; rotation will proceed"), *GetOwner()->GetName());

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
	
    AActor* Owner = GetOwner();
    if (!Owner) return;

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
// 
//     PRINTLOG(TEXT("Rotating %s by %f degrees"), *Owner->GetName(), RotateSpeed * DeltaTime);
}

void UMyRotateActorComponent::StartRolling()
{
     bShouldRot = true;  // 회전 시작을 위한 플래그 설정

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

     SRPC_SetOwnerSync(RotateDirection,RotateSpeed);
//     TotalRotationAngle = 0.0f;
    UE_LOG(LogTemp, Warning, TEXT("StartRolling called and bShouldRot set to true for %s"), *GetOwner()->GetName());
}

void UMyRotateActorComponent::StopRolling()
{
    SRPCStopRolling();

    APSH_BlockActor* block = Cast<APSH_BlockActor>(GetOwner());

}

void UMyRotateActorComponent::SRPCStopRolling_Implementation()
{
    MRPC_StopRolling();
}
void UMyRotateActorComponent::MRPC_StopRolling_Implementation()
{
    bShouldRot = false;
    TotalRotationAngle = 0.0f;  // 회전 초기화
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
void UMyRotateActorComponent::GetDelegateBool(bool delegatebool)
{
    bShouldRot = !delegatebool;

 
        APSH_BlockActor* block = Cast<APSH_BlockActor>(GetOwner());
        if (block->ActorHasTag(FName("owner")))
            block->SRPC_SetSimulatePhysics(false);
  
}
FPSH_FunctionBlockData UMyRotateActorComponent::SaveData()
{
    FPSH_FunctionBlockData funtionData;
    PRINTLOG(TEXT("UMyRotateActorComponent::SaveData()"));
  
    funtionData.floatArray.Add(RotateSpeed);
    funtionData.frotatorArray.Add(RotateDirection);

    return funtionData;
}
void UMyRotateActorComponent::LoadData(FPSH_FunctionBlockData funtionData)
{
    RotateSpeed = funtionData.floatArray[0]; //float
    RotateDirection = funtionData.frotatorArray[0]; //FRotator
  
}

void UMyRotateActorComponent::SetOwnerRotation_Implementation(const FQuat& newRotation)
{
    if (GetOwner() == nullptr) return;

    GetOwner()->SetActorRotation(newRotation);

//     if (GetOwner()->GetOwner() == nullptr)
//     {
//         PRINTLOG(TEXT("Owner == Null"));
//     }
//     else
//     {
//         PRINTLOG(TEXT("Owner : %s"), *GetOwner()->GetOwner()->GetName());
//     }
}

void UMyRotateActorComponent::SRPC_SetOwnerSync_Implementation(FRotator CRotateDirection, float CRotateSpeed)
{
    RotateDirection = CRotateDirection;
    RotateSpeed = CRotateSpeed;
}
void UMyRotateActorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

     DOREPLIFETIME(UMyRotateActorComponent, RotateDirection);
     DOREPLIFETIME(UMyRotateActorComponent, RotateSpeed);
     DOREPLIFETIME(UMyRotateActorComponent, bShouldRot);
 }