// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/PSH_Player/PSH_PlayerController.h"
#include "Kismet/KismetMathLibrary.h"

APSH_PlayerController::APSH_PlayerController()
{
	
}
void APSH_PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

// 	if(targetLockOn)
// 	LookMouseCursor();
}
void APSH_PlayerController::BeginPlay()
{
	Super::BeginPlay();

		SetShowMouseCursor(true);
		bEnableClickEvents = true;
		bEnableMouseOverEvents = true;
		SetInputMode(FInputModeGameAndUI());
}
void APSH_PlayerController::LookMouseCursor()
{
	FHitResult hit;
	GetHitResultUnderCursor(ECC_Visibility,false,hit);

	if (hit.bBlockingHit) // 라인 트레이스가 물체와 부딫혔을 경우
	{
		APawn * MyPawn = GetPawn();
		if (MyPawn)
		{
			FVector WorldLoc;
			FVector WorldDir;

			DeprojectMousePositionToWorld(WorldLoc, WorldDir); // 마우스 좌표 변환
// 			WorldLoc.Normalize();
// 			WorldDir.Normalize();

			FRotator lookRotation = UKismetMathLibrary::FindLookAtRotation(MyPawn->GetActorLocation(), FVector(WorldLoc.X,WorldLoc.Y, MyPawn->GetActorLocation().Z));

			FRotator lookRotation3 = UKismetMathLibrary::FindLookAtRotation(MyPawn->GetActorLocation(), FVector(WorldDir.X,WorldDir.Y, MyPawn->GetActorLocation().Z));


			FRotator  lookRot = WorldDir.Rotation();
			lookRot.Pitch = 0;
			UE_LOG(LogTemp,Warning,TEXT("WorldLoc : %f,%f,%f"), WorldLoc.X, WorldLoc.Y, WorldLoc.Z);
			UE_LOG(LogTemp,Warning,TEXT("WorldDir : %f,%f,%f"), WorldDir.X, WorldDir.Y, WorldDir.Z);
			UE_LOG(LogTemp,Warning,TEXT("hit.Location : %f,%f,%f"), hit.Location.X, hit.Location.Y, hit.Location.Z);


			FRotator lookRotation2 = UKismetMathLibrary::FindLookAtRotation(MyPawn->GetActorLocation(),
				FVector(hit.Location.X, hit.Location.Y,MyPawn->GetActorLocation().Z));

			MyPawn->SetActorRotation(lookRot);
		}
	}
}