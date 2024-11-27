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
#include "CSR/DedicatedServer/AutoGameState.h"
#include "CSR/DedicatedServer/AutoRoomManager.h"

APSH_PlayerController::APSH_PlayerController()
{
	// 생성자에 블루프린트 클래스 로드
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
			FPSH_ObjectData BlockData = actor->SaveBlock();
//			dataTable->AddRow(rowName, BlockData);
			UE_LOG(LogTemp,Warning,TEXT("Save RowNam : %d"),RowNum);
		}

	}
	//SaveTheGame();
}
void APSH_PlayerController::ObjectLoad()
{
// 	UE_LOG(LogTemp, Warning, TEXT("Load RowNam : %d"), RowNum);
// 
// 	TArray<FPSH_ObjectData*> dataAraay;
// 	dataTable->GetAllRows<FPSH_ObjectData>(TEXT("non"), dataAraay);
// 
// 	for (int i = 0; i < dataAraay.Num(); i++)
// 	{
// 		if (!dataAraay.IsEmpty() && dataAraay[i]->actor != nullptr)
// 		{
// 			// 루트 블럭 소환
// 			TSubclassOf<APSH_BlockActor> SpawnActor = dataAraay[i]->actor;
// 			if (SpawnActor)
// 			{
// 				FActorSpawnParameters Params;
// 				APSH_BlockActor* SpawnedBlock = GetWorld()->SpawnActor<APSH_BlockActor>(SpawnActor, dataAraay[i]->actorTransfrom, Params);
// 
// 				// 블럭 계층 구조 불러오기
// 				if (SpawnedBlock)
// 				{
// 					//SpawnedBlock->LoadBlockHierarchy(*dataAraay[i]);
// 				}
// 			}
// 		}
// 		
// 	}
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

void APSH_PlayerController::SelectObject(AActor* SelectedActor)
{
	//UE_LOG(LogTemp, Warning, TEXT("SelectObject called for Actor: %s"), *SelectedActor->GetName());

	//APSH_BlockActor* BlockActor = Cast<APSH_BlockActor>(SelectedActor);

	//// 선택한 액터가 BlockActor이고, 필요한 컴포넌트를 가지고 있을 때만 UI 열기
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
	//		return; // UI가 열렸으므로 함수 종료
	//	}
	//}

	//// 조건이 충족되지 않을 경우 UI를 닫기
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

// 서버에 룸 관련 요청 - 성락
#pragma region
void APSH_PlayerController::ServerRequestCreateAutoRoom_Implementation(const FString& RoomName, const FString& JsonData)
{
	if (!HasAuthority())
	{
		return;
	}

	if (AAutoGameState* GameState = GetWorld()->GetGameState<AAutoGameState>())
	{
		if (AAutoRoomManager* RoomManager = GameState->AutoRoomManager)
		{
			if (JsonData.IsEmpty())
			{
				// Create 모드 - 빈 방 생성
				RoomManager->CreateAutoRoom(RoomName, this);
			}
			else
			{
				// Play 모드 - JSON 데이터로 방 생성
				RoomManager->CreateAutoRoomWithData(RoomName, JsonData, this);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AutoRoomManager not found in GameState"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameState not found"));
	}
}

void APSH_PlayerController::ServerRequestJoinAutoRoom_Implementation(const FString& RoomName)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerRequestJoinAutoRoom: No Authority"));
		return;
	}

	AAutoGameState* GameState = GetWorld()->GetGameState<AAutoGameState>();
	if (!GameState)
	{
		UE_LOG(LogTemp, Error, TEXT("ServerRequestJoinAutoRoom: GameState not found"));
		return;
	}

	if (!GameState->AutoRoomManager)
	{
		UE_LOG(LogTemp, Error, TEXT("ServerRequestJoinAutoRoom: AutoRoomManager not found in GameState"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("ServerRequestJoinAutoRoom: Joining room %s"), *RoomName);
	GameState->AutoRoomManager->JoinAutoRoom(RoomName, this);
}

void APSH_PlayerController::ServerRequestLeaveAutoRoom_Implementation(const FString& RoomName)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerRequestJoinAutoRoom: No Authority"));
		return;
	}

	AAutoGameState* GameState = GetWorld()->GetGameState<AAutoGameState>();
	if (!GameState)
	{
		UE_LOG(LogTemp, Error, TEXT("ServerRequestJoinAutoRoom: GameState not found"));
		return;
	}

	if (!GameState->AutoRoomManager)
	{
		UE_LOG(LogTemp, Error, TEXT("ServerRequestJoinAutoRoom: AutoRoomManager not found in GameState"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("ServerRequestJoinAutoRoom: Leaving room %s"), *RoomName);
	GameState->AutoRoomManager->LeaveAutoRoom(RoomName, this);
}

TArray<FPSH_ObjectData*> APSH_PlayerController::ParseJsonToObjectData(const FString& JsonString)
{
	TArray<FPSH_ObjectData*> DataArray;

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		const TArray<TSharedPtr<FJsonValue>>* DataTableArray;
		if (JsonObject->TryGetArrayField(TEXT("data_table"), DataTableArray))
		{
			for (const auto& Value : *DataTableArray)
			{
				FPSH_ObjectData* ObjectData = new FPSH_ObjectData();
				const TSharedPtr<FJsonObject>* RowObject;

				if (Value->TryGetObject(RowObject))
				{
					if (FJsonObjectConverter::JsonObjectToUStruct((*RowObject).ToSharedRef(), ObjectData))
					{
						DataArray.Add(ObjectData);
					}
				}
			}
		}
	}

	return DataArray;
}

//void APSH_PlayerController::ShowMapSaveUI()
//{
//	if (HasAuthority()) {
//		return ;
//	}
//	if (!MapCreateWidget && CreateWidgetClass)
//	{
//		MapCreateWidget = CreateWidget<UWBP_CreateWidget>(this, CreateWidgetClass);
//	}
//
//	if (MapCreateWidget)
//	{
//		MapCreateWidget->AddToViewport();
//	}
//}
//
//#pragma endregion
//
