// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/PSH_Player/PSH_PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PSH/PSH_Actor/PSH_BlockActor.h"
#include "PSH/PSH_Player/PSH_Player.h"

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

		curPlayer = Cast<APSH_Player>(GetPawn());
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
void APSH_PlayerController::ObjectSave()
{
	TArray<AActor*> blackArray;
	

	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APSH_BlockActor::StaticClass(),FName(TEXT("owner")), blackArray);

	for (auto* arrayActor : blackArray)
	{
		APSH_BlockActor * actor = Cast<APSH_BlockActor>(arrayActor);

		if (actor)
		{
			FName rowName = FName(*FString::FormatAsNumber(RowNum++));
			FPSH_ObjectData BlockData = actor->SaveBlockHierachy();
			dataTable->AddRow(rowName, BlockData);
			UE_LOG(LogTemp,Warning,TEXT("Save RowNam : %d"),RowNum);
		}

	}
}
void APSH_PlayerController::ObjectLoad()
{
	UE_LOG(LogTemp, Warning, TEXT("Load RowNam : %d"), RowNum);

	TArray<FPSH_ObjectData*> dataAraay;
	dataTable->GetAllRows<FPSH_ObjectData>(TEXT("non"), dataAraay);

	for (int i = 0; i < dataAraay.Num(); i++)
	{
		if (!dataAraay.IsEmpty() && dataAraay[i]->actor != nullptr)
		{
			// 루트 블럭 소환
			TSubclassOf<APSH_BlockActor> SpawnActor = dataAraay[i]->actor;
			if (SpawnActor)
			{
				FActorSpawnParameters Params;
				APSH_BlockActor* SpawnedBlock = GetWorld()->SpawnActor<APSH_BlockActor>(SpawnActor, dataAraay[i]->actorTransfrom, Params);

				// 블럭 계층 구조 불러오기
				if (SpawnedBlock)
				{
					SpawnedBlock->LoadBlockHierarchy(*dataAraay[i]);
				}
			}
		}
		
	}
// 
// 	for (int i = RowNum; i >= 0; i--)
// 	{
// 		FName rowName = FName(*FString::FormatAsNumber(i));
// 		FPSH_ObjectData* data = dataTable->FindRow<FPSH_ObjectData>(rowName, TEXT("non"));
// 		APSH_BlockActor* sapwnPartent = nullptr;
// 
// 		if (data && data->actor != nullptr)
// 		{
// 			// 루트 블럭 소환
// 			TSubclassOf<APSH_BlockActor> SpawnActor = data->actor;
// 			if (SpawnActor)
// 			{
// 				FActorSpawnParameters Params;
// 				APSH_BlockActor* SpawnedBlock = GetWorld()->SpawnActor<APSH_BlockActor>(SpawnActor, data->actorTransfrom, Params);
// 
// 				// 블럭 계층 구조 불러오기
// 				if (SpawnedBlock)
// 				{
// 					SpawnedBlock->LoadBlockHierarchy(*data);
// 				}
// 			}
// 		}
// 		UE_LOG(LogTemp, Warning, TEXT("%d"), RowNum);
// 	}
// 	
}