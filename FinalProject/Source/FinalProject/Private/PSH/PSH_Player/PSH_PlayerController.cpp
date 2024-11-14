// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/PSH_Player/PSH_PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PSH/PSH_Actor/PSH_BlockActor.h"
#include "YWK/MyChoiceActionWidget.h"
#include "YWK/MyFlyActorComponent.h"
#include "YWK/MyMoveActorComponent.h"
#include "PSH/PSH_Player/PSH_Player.h"
#include "Blueprint/UserWidget.h"
#include "CSR/UI/CSR_Proto_StartUI.h"
#include "PSH/PSH_UI/PSH_ObjectWidget.h"
#include "../FinalProject.h"

APSH_PlayerController::APSH_PlayerController()
{
	// �����ڿ� �������Ʈ Ŭ���� �ε�
	static ConstructorHelpers::FClassFinder<UMyChoiceActionWidget> WidgetClassFinder(TEXT("/Game/YWK/UI/WBP_Choice.WBP_Choice_C"));
	if (WidgetClassFinder.Succeeded())
	{
		ChoiceWidgetClass = WidgetClassFinder.Class;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find WBP_Choice at specified path"));
	}
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
		//curPlayer = Cast<APSH_Player>(GetPawn());
		//curPlayer->SetActorHiddenInGame(true);
		//this->SetInputMode(FInputModeUIOnly());
// 		StartUI = CreateWidget<UCSR_Proto_StartUI>(this, StartUIFac);
// 		StartUI->AddToViewport();
		SetInputMode(FInputModeGameAndUI());

}
void APSH_PlayerController::LookMouseCursor()
{
	FHitResult hit;
	GetHitResultUnderCursor(ECC_Visibility,false,hit);

	if (hit.bBlockingHit) // ���� Ʈ���̽��� ��ü�� �΋H���� ���
	{
		APawn * MyPawn = GetPawn();
		if (MyPawn)
		{
			FVector WorldLoc;
			FVector WorldDir;

			DeprojectMousePositionToWorld(WorldLoc, WorldDir); // ���콺 ��ǥ ��ȯ
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
	SaveTheGame();
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
			// ��Ʈ �� ��ȯ
			TSubclassOf<APSH_BlockActor> SpawnActor = dataAraay[i]->actor;
			if (SpawnActor)
			{
				FActorSpawnParameters Params;
				APSH_BlockActor* SpawnedBlock = GetWorld()->SpawnActor<APSH_BlockActor>(SpawnActor, dataAraay[i]->actorTransfrom, Params);

				// �� ���� ���� �ҷ�����
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
// 			// ��Ʈ �� ��ȯ
// 			TSubclassOf<APSH_BlockActor> SpawnActor = data->actor;
// 			if (SpawnActor)
// 			{
// 				FActorSpawnParameters Params;
// 				APSH_BlockActor* SpawnedBlock = GetWorld()->SpawnActor<APSH_BlockActor>(SpawnActor, data->actorTransfrom, Params);
// 
// 				// �� ���� ���� �ҷ�����
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

void APSH_PlayerController::SelectObject(AActor* SelectedActor)
{
	//UE_LOG(LogTemp, Warning, TEXT("SelectObject called for Actor: %s"), *SelectedActor->GetName());

	//APSH_BlockActor* BlockActor = Cast<APSH_BlockActor>(SelectedActor);

	//// ������ ���Ͱ� BlockActor�̰�, �ʿ��� ������Ʈ�� ������ ���� ���� UI ����
	//if (BlockActor)
	//{
	//	UMyMoveActorComponent* MoveComponent = BlockActor->FindComponentByClass<UMyMoveActorComponent>();
	//	UMyFlyActorComponent* FlyComponent = BlockActor->FindComponentByClass<UMyFlyActorComponent>();

	//	if (MoveComponent || FlyComponent)
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("BlockActor has active Move or Fly component"));

	//		if (ChoiceWidgetClass && (!MyChoiceActionWidget || !MyChoiceActionWidget->IsInViewport()))
	//		{
	//			MyChoiceActionWidget = CreateWidget<UMyChoiceActionWidget>(this, ChoiceWidgetClass);
	//			if (MyChoiceActionWidget)
	//			{
	//				MyChoiceActionWidget->AddToViewport();
	//				MyChoiceActionWidget->SetVisibility(ESlateVisibility::Visible);
	//				UE_LOG(LogTemp, Warning, TEXT("MyChoiceActionWidget set to Visible and added to viewport"));
	//			}
	//		}
	//		return; // UI�� �������Ƿ� �Լ� ����
	//	}
	//}

	//// ������ �������� ���� ��� UI�� �ݱ�
	//if (MyChoiceActionWidget && MyChoiceActionWidget->IsInViewport())
	//{
	//	MyChoiceActionWidget->RemoveFromParent();
	//	MyChoiceActionWidget = nullptr;
	//	UE_LOG(LogTemp, Warning, TEXT("MyChoiceActionWidget removed from viewport"));
	//}
}
void APSH_PlayerController::CRPC_ShowObjectWidget_Implementation()
{
	if(objectWidget == nullptr ) return;

	PRINTLOG(TEXT("Show objectWidget"));

	if (objectWidget->IsVisible() == false)
	{
		objectWidget->SetVisibility(ESlateVisibility::Visible);
		objectWidget->StartSpawnAnim();
	}
}

void APSH_PlayerController::CRPC_CloseObjectWidget_Implementation()
{
	if (objectWidget == nullptr) return;

	if (objectWidget->IsVisible() )
	{
		objectWidget->EndSpawnAnim();
		/*objectWidget->SetVisibility(ESlateVisibility::Hidden);*/
	}
}
void APSH_PlayerController::CRPC_ObjectWidgetVisible_Implementation(bool check)
{
	if (objectWidget == nullptr) return;

	if (check)
	{
		if (objectWidget->IsVisible() == false)
		{
			objectWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		if (objectWidget->IsVisible())
		{
			objectWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	
}	