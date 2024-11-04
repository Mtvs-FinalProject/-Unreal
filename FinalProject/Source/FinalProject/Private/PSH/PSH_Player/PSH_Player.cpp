// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/PSH_Player/PSH_Player.h"
#include "PSH/PSH_Actor/PSH_BlockActor.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PSH/PSH_UI/PSH_MouseWidget.h"
#include "PSH/PSH_Player/PSH_PlayerController.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetArrayLibrary.h"
#include "PSH/PSH_DataTable/PSH_MechDataTable.h"
#include "Engine/DataTable.h"
#include "PSH/PSH_Actor/PSH_GarbageBot.h"
#include "Components/WidgetComponent.h"
#include "PSH/PSH_UI/PSH_GarbageBotWidget.h"
#include "../FinalProject.h"
#include "Net/UnrealNetwork.h"
#include "YWK/MyMoveActorComponent.h"
#include "YWK/MyFlyActorComponent.h"
#include "YWK/MyChoiceActionWidget.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APSH_Player::APSH_Player()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 스프링 컴포넌트
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	springArm->SetupAttachment(RootComponent);

	// 카메라 컴포넌트
	rotationHelper = CreateDefaultSubobject<USceneComponent>(TEXT("RotationHelper"));
	rotationHelper->SetupAttachment(RootComponent);

	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	cameraComp->SetupAttachment(springArm);

	// 피직스 핸들 컴포넌트
	handleComp = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("Handle"));

	previewMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("previewMesh"));
	previewMeshComp->SetupAttachment(RootComponent);

	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void APSH_Player::BeginPlay()
{
	Super::BeginPlay();
	previewMeshComp->OnComponentBeginOverlap.AddDynamic(this,&APSH_Player::PreviewMeshBeginOverlap);
	previewMeshComp->SetVisibility(false);
	// 마우스 위젯 사용 
	mouseWidget = Cast<UPSH_MouseWidget>(CreateWidget(GetWorld(), mouseWidgetFac));
	botWidget  = Cast<UPSH_GarbageBotWidget>(CreateWidget(GetWorld(), botWidgetFac));

	if (IsLocallyControlled())
	{
		pc = Cast<APSH_PlayerController>(GetController());
	}

	if (botWidget)
	{
		botWidget->AddToViewport();
		botWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	if (mouseWidget)
	{
		if (pc)
		{
			pc->SetMouseCursorWidget(EMouseCursor::Default, mouseWidget);
		}
	}

}

// Called every frame
void APSH_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (pc && pc->IsLocalController())
	{
	
		// 잡은게 없다면 
		if(handleComp->GetGrabbedComponent() == nullptr) return;

		// 잡은애 블럭이 없지 않다면
		if (handleComp->GetGrabbedComponent()->GetOwner() != nullptr)
		{
			auto* snap = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner());
			if(snap)
			snapPointIndexLength = snap->GetSnapPoints().Num();
		}

		FLocationPoint point; 
		PreTraceCheck(point.startLocation, point.endLocation); // 206 과 58 줄 널

		ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);
		FHitResult outHit;

		bool hit = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			point.startLocation,
			point.endLocation,
			TraceChannel,
			false,
			actorsToIgnore,
			EDrawDebugTrace::None,
			outHit,
			true,
			FColor::Green,
			FColor::Red,
			4
		);

		HandleBlock(outHit, hit, point.endLocation);
	}
	
}

// Called to bind functionality to input
void APSH_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	pc = Cast<APSH_PlayerController>(GetController());

	if (pc)
	{
		UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());

		if (subsystem)
		{
			subsystem->AddMappingContext(imc_Player, 0);
		}
	}

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent != nullptr)
	{
		EnhancedInputComponent->BindAction(inputActions[0], ETriggerEvent::Triggered, this, &APSH_Player::Move);
		EnhancedInputComponent->BindAction(inputActions[1], ETriggerEvent::Triggered, this, &APSH_Player::Look);
		EnhancedInputComponent->BindAction(inputActions[2], ETriggerEvent::Started, this, &APSH_Player::Grab);
		EnhancedInputComponent->BindAction(inputActions[3], ETriggerEvent::Started, this, &APSH_Player::PlayerJump);

		// 스폰 블럭
		EnhancedInputComponent->BindAction(inputActions[4], ETriggerEvent::Started, this, &APSH_Player::SpawnBlock);

		// 블럭 저장
		EnhancedInputComponent->BindAction(inputActions[5], ETriggerEvent::Started, this, &APSH_Player::SaveTest);

		// 데이터 테이블 스폰 블럭
		EnhancedInputComponent->BindAction(inputActions[6], ETriggerEvent::Started, this, &APSH_Player::LoadTest);

		// 인터페이스 관련
		EnhancedInputComponent->BindAction(inputActions[7], ETriggerEvent::Started, this, &APSH_Player::ShowInterface);

		// 좌 우로 45도 돌리기
		EnhancedInputComponent->BindAction(inputActions[8], ETriggerEvent::Started, this, &APSH_Player::HorizontalRotChange);
		
		// 위 아래로 45도 돌리기
		EnhancedInputComponent->BindAction(inputActions[9], ETriggerEvent::Started, this, &APSH_Player::VerticalRotChange);
		
		// Art확인
		EnhancedInputComponent->BindAction(inputActions[10], ETriggerEvent::Started, this, &APSH_Player::OnArtKey);
		EnhancedInputComponent->BindAction(inputActions[10], ETriggerEvent::Completed, this, &APSH_Player::OnArtKey);

		// bot 모드와 이동 관리
		EnhancedInputComponent->BindAction(inputActions[11], ETriggerEvent::Started, this, &APSH_Player::BotMoveAndModeChange);
		
	}

}
void APSH_Player::Move(const FInputActionValue& value)
{
	FVector2D input2D = value.Get<FVector2D>();

	FVector forwardVec = FRotationMatrix(pc->GetControlRotation()).GetUnitAxis(EAxis::X);
	FVector rightVec = FRotationMatrix(pc->GetControlRotation()).GetUnitAxis(EAxis::Y);


	AddMovementInput(forwardVec, input2D.X);
	AddMovementInput(rightVec, input2D.Y);
}
void APSH_Player::Look(const FInputActionValue& value)
{
	FVector2D input2D = value.Get<FVector2D>();

	AddControllerPitchInput(input2D.Y);
	AddControllerYawInput(input2D.X);
}
void APSH_Player::PlayerJump()
{
	Jump();
}

void APSH_Player::SetPreviewMesh(class UStaticMesh* previewMesh , TSubclassOf<class APSH_BlockActor> spawnActor)
{
	
	if (!previewMeshComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("previewMeshComp is not initialized."));
		return;
	}
	previewMeshComp->SetVisibility(true);
	// 2. 미리보기 메쉬 할당
	if (previewMesh)
	{
		previewMeshComp->SetStaticMesh(previewMesh);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PreviewMesh is null."));
		return;
	}

	blockSpawner = spawnActor;
	// 3. 캐릭터의 위치와 방향을 기준으로 상대적 위치 설정
	FVector CharacterLocation = GetActorLocation();
	FVector RightVector = GetActorRightVector();
	FVector ForwardVector = GetActorForwardVector();

	FVector SpawnLocation = CharacterLocation + (RightVector * 200) + (ForwardVector * 100);

	// 4. 월드 좌표 설정
	previewMeshComp->SetWorldLocation(SpawnLocation);

	// 5. 메쉬에 머테리얼 할당
	previewMeshComp->SetMaterial(0, previewMat);

	// 디버그 메시지 출력
	UE_LOG(LogTemp, Log, TEXT("Preview mesh set at location: %s"), *SpawnLocation.ToString());
}
void APSH_Player::PreviewMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 오버렙 시 소환 불가 .
}
void APSH_Player::Grab()
{
	if (handleComp == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("NullHandle"))
			return;
	}

	if (handleComp->GetGrabbedComponent() != nullptr) // 잡은 게 있다면
	{

		FLocationPoint point;
		PreTraceCheck(point.startLocation, point.endLocation);

		ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);
		FHitResult OutHit;

		bool hit = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			point.startLocation,
			point.endLocation,
			TraceChannel,
			false,
			actorsToIgnore,
			EDrawDebugTrace::None,
			OutHit,
			true,
			FColor::Green,
			FColor::Red,
			4
		);
		
		PlaceBlock(OutHit, hit); // 멀티 , 서버화 안되어 있음.
	}
	else // 잡은게 없다면
	{
		CastRay();
	}
}

void APSH_Player::PreTraceCheck( FVector& StartLoc,  FVector& EndLoc) // 
{
	
	if (handleComp->GetGrabbedComponent() == nullptr) return;

	APSH_BlockActor* ChildBlcak = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner()); // Array에 저장

	if(ChildBlcak == nullptr) return;

	FVector WorldLoc;
	FVector WorldDir;

	pc->DeprojectMousePositionToWorld(WorldLoc, WorldDir); // 마우스 좌표 변환

	actorsToIgnore = ChildBlcak->childsActors;
	actorsToIgnore.Add(handleComp->GetGrabbedComponent()->GetOwner());
	
	FVector CameraLoc = cameraComp->GetComponentLocation();
	StartLoc = CameraLoc;
	EndLoc = CameraLoc + playerReach * WorldDir;

}


void APSH_Player::CastRay() // 잡기위한 레이 
{

	FVector worldLoc;
	FVector worldDir;

	pc->DeprojectMousePositionToWorld(worldLoc, worldDir);

	FVector StartLoc = cameraComp->GetComponentLocation(); // 터짐
	FVector EndLoc = StartLoc + (rayPower * worldDir);
	
	if (IsLocallyControlled() == false) return;

	SRPC_Pickup(StartLoc,EndLoc);
}

void APSH_Player::SRPC_Pickup_Implementation(const FVector& startLoc, const FVector& endLoc) // 맞은 위치가 애매하다?
{
	
	FHitResult hitInfo;

	FCollisionQueryParams prams;
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startLoc, endLoc, ECC_Visibility, prams);

	if (bHit)
	{
		APSH_BlockActor* target = Cast<APSH_BlockActor>(hitInfo.GetActor());
		if (target)
		{
			if (target->GetOwner() == nullptr)
			{
				target->SetOwner(this);
				target->PickUp(handleComp);
			}
		}
	}
	
}
void APSH_Player::ClosestPoint(TArray<FVector> pointArray, FVector testLocation, FTransform hitActorTransfrom ,
								FVector & closestPt, float& dist , int32& closetPointIndex)
{
	if (pointArray.Num() == 0) return;

	FVector testLoc = UKismetMathLibrary::InverseTransformLocation(hitActorTransfrom,testLocation);


	
		closestPt = pointArray[0]; //

		dist = FVector::Dist(closestPt, testLoc); //

		closetPointIndex = 0; //
		
	// 까지 한번만 반복
	
	for (int32 i = 0 ; i < pointArray.Num(); i++) // 가장 가까운 포인트 찾기
	{
		float currentDist = (FVector::Dist(pointArray[i], testLoc));
		if  (currentDist < dist)
		{
			closestPt = pointArray[i];
			dist = currentDist;
			closetPointIndex = i;
			
		}
	} 
	
}
FRotator APSH_Player::WorldHelperRotationOffset()
{
	TArray<FRotator> snapDir = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner())->GetSnapDirections();

	return UKismetMathLibrary::ComposeRotators(UKismetMathLibrary::NegateRotator(snapDir[snapPointIndex]),FRotator(180,180,0));

}

// 현재 서버 클라 다불림.
// 서버에서 불려야하지않나?
void APSH_Player::PlaceBlock(FHitResult hitInfo, bool hit)
{
	SRPC_PlaceBlock(hitInfo,hit);
}	

void APSH_Player::SRPC_PlaceBlock_Implementation(FHitResult hitInfo, bool hit)
{
	APSH_BlockActor* heldBlock = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner());
	PRINTLOG(TEXT("PlaceBlock"));
	if (hit)
	{
		FVector location = hitInfo.Location;
		FVector normal = hitInfo.Normal;
		APSH_BlockActor* actor = Cast<APSH_BlockActor>(hitInfo.GetActor());

		if (Cast<APSH_BlockActor>(actor))
		{
			TArray<FVector> snapPoints = actor->GetSnapPoints();
			TArray<FRotator> snapDirections = actor->GetSnapDirections();

			FVector closestPoint = FVector::ZeroVector;
			float distance = 0;
			int32 arrayindex = 0;

			ClosestPoint(snapPoints, location, actor->GetActorTransform(), closestPoint, distance, arrayindex);

			TArray<FVector> heldBlockPoints = heldBlock->GetSnapPoints();

			bool distBool = distance <= snapDistance;
			bool bValidPoint = snapPoints.IsValidIndex(0);
			bool andChek = distBool && bValidPoint;



			FVector snapLocation;
			if (!andChek)
			{
				snapLocation = location + (heldBlock->GetActorLocation() -
					UKismetMathLibrary::TransformLocation(heldBlock->GetActorTransform(), heldBlockPoints[snapPointIndex]));
			}
			else
			{
				snapLocation = UKismetMathLibrary::TransformLocation(actor->GetActorTransform(), closestPoint) +
					(heldBlock->GetActorLocation() -
						UKismetMathLibrary::TransformLocation(heldBlock->GetActorTransform(), heldBlockPoints[snapPointIndex]));
			}

			FTransform worldTransfrom;


			worldTransfrom = UKismetMathLibrary::MakeTransform(UKismetMathLibrary::InverseTransformLocation(
				actor->GetActorTransform(), snapLocation),
				rotationHelper->GetComponentRotation());

			if (actor->connectionsize >= heldBlock->connectionsize)
			{
				if (heldBlock->OvelapChek())
				{
					heldBlock->Place(actor, worldTransfrom);
					DropBlcok();
					snapPointIndex = 0;
				}
				else
				{
					if (GEngine)
						GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("what?")));
					return;
				}
			}
			else
			{
				DropBlcok();
			}
		}
		else
		{
			if (heldBlock->OvelapChek())
			{
				DropBlcok();
			}
			else
			{
				if (GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Not Cast , OvelapChek : Faslse")));
				return;
			}
		}
	}
	else
	{
		if (heldBlock->OvelapChek())
		{
			DropBlcok();
		}
		else
		{
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("hit : else , OvelapChek : Faslse")));
			return;
		}
	}
}
void APSH_Player::DropBlcok()
{
	SRPC_DropBlcok();
}

void APSH_Player::MRPC_DropBlcok_Implementation()
{
	Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner())->Drop(handleComp);
}
void APSH_Player::SRPC_DropBlcok_Implementation()
{
	MRPC_DropBlcok(); 
	// 저장 하는 무언가 필요.
	
}
void APSH_Player::HandleBlock(FHitResult hitinfo, bool hit, FVector rayEndLocation)
{
	SRPC_HandleBlock(hitinfo, hit, rayEndLocation);
}
void APSH_Player::SRPC_HandleBlock_Implementation(FHitResult hitinfo, bool hit, FVector rayEndLocation)
{
	if (handleComp->GetGrabbedComponent() == nullptr) return;

	snapPointIndexLength = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner())->GetSnapPoints().Num();

	if (hit)
	{
		FVector localLocation = hitinfo.Location;
		FVector localNormal = hitinfo.Normal;
		auto* hitActor = Cast<APSH_BlockActor>(hitinfo.GetActor()); // ray에 충돌한 엑터
		FTransform hitActorTransfrom = hitinfo.GetActor()->GetActorTransform();

		TArray<FVector> snapPoints; // 조인트 위치값
		TArray<FRotator> snapDirection; // 조인트 방향

		FVector closestPoint = FVector::ZeroVector;
		float distance = 0;
		int32 ClosestSnapPointIndex = 0;

		FVector newLoc; // 새로운 위치
		FRotator newRot; // 새로운 방향

		auto* heldActor = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner()); // 잡은 엑터
		if (Cast<APSH_BlockActor>(hitActor)) // 범위 안에 있는 상태
		{
			if (hitActor->GetSnapPoints().IsEmpty()) return; // 비어있다면

			snapPoints = hitActor->GetSnapPoints();

			ClosestPoint(snapPoints, localLocation, hitActorTransfrom, closestPoint, distance, ClosestSnapPointIndex);
			snapDirection = hitActor->GetSnapDirections();
			Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner())->OvelapChek();
		}

		bool bDist = snapDistance >= distance;
		bool bValidPoint = snapPoints.IsValidIndex(0);
		bool andBool = bDist && bValidPoint && hit;

		if (heldActor == nullptr) return;

		snapPoints = heldActor->GetSnapPoints();

		if (!andBool)
		{
			newLoc = localLocation + (heldActor->GetActorLocation() -
				UKismetMathLibrary::TransformLocation(heldActor->GetActorTransform(), snapPoints[snapPointIndex]));
		}
		else
		{
			newLoc = UKismetMathLibrary::TransformLocation(hitActorTransfrom, closestPoint) +
				(heldActor->GetActorLocation() - UKismetMathLibrary::TransformLocation(heldActor->GetActorTransform(), snapPoints[snapPointIndex]));
		}

		rotationHelper->SetWorldLocation(newLoc);

		if (!andBool)
		{
			newRot = UKismetMathLibrary::MakeRotFromZ(localNormal);
		}
		else
		{
			newRot = UKismetMathLibrary::TransformRotation(hitActorTransfrom, snapDirection[ClosestSnapPointIndex]);
		}

		rotationHelper->SetWorldRotation(newRot);

		rotationHelper->AddLocalRotation(WorldHelperRotationOffset());

		FRotator A = UKismetMathLibrary::RotatorFromAxisAndAngle(UKismetMathLibrary::GetRightVector(newRot), rotationOffset.Pitch);
		FRotator B = UKismetMathLibrary::RotatorFromAxisAndAngle(UKismetMathLibrary::GetUpVector(newRot), rotationOffset.Yaw);

		rotationHelper->AddWorldRotation(UKismetMathLibrary::ComposeRotators(A, B));
		ReplicatedLocation = rotationHelper->GetComponentLocation();
		ReplicatedRotation = rotationHelper->GetComponentRotation();
		//handleComp->SetTargetLocationAndRotation(rotationHelper->GetComponentLocation(), rotationHelper->GetComponentRotation());
	}
	else
	{
		ReplicatedLocation = rayEndLocation;
		ReplicatedRotation = rotationOffset;
		//handleComp->SetTargetLocationAndRotation(rayEndLocation, rotationOffset);
	}

	MRPC_HandleBlock(ReplicatedLocation,ReplicatedRotation);
}
void APSH_Player::MRPC_HandleBlock_Implementation(FVector newLoc, FRotator newRot)
{
	handleComp->SetTargetLocationAndRotation(newLoc, newRot);
}


void APSH_Player::SpawnBlock()
{

}

void APSH_Player::SRPC_SpawnBlock_Implementation()
{

	// 캐릭터의 위치와 방향을 기준으로 상대적 위치를 설정
	FVector CharacterLocation = GetActorLocation();
	FVector RightVector = GetActorRightVector();  // 캐릭터의 오른쪽 방향
	FVector ForwardVector = GetActorForwardVector();  // 캐릭터의 앞쪽 방향

	// 상대적 오프셋 설정 (오른쪽으로 200, 앞쪽으로 약간 떨어진 위치)
	FVector SpawnLocation = CharacterLocation + (RightVector * 200) + (ForwardVector * 100);

	// 스폰 파라미터 설정 및 엑터 소환
	FActorSpawnParameters SpawnParams;
	APSH_BlockActor* SpawnedActor = GetWorld()->SpawnActor<APSH_BlockActor>(blockSpawner, SpawnLocation, GetActorRotation(), SpawnParams);

}

void APSH_Player::SaveTest()
{
// 		
// 		if (handleComp->GetGrabbedComponent()->GetOwner() == nullptr) return; 
// 
// 		APSH_BlockActor* actor = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner()); // 잡은 블럭
// 		FName rowName = FName(*FString::FormatAsNumber(4));
// 		if (actor && dataTable)
// 		{
// 			FPSH_ObjectData BlockData = actor->SaveBlockHierachy();
// 			dataTable->AddRow(rowName, BlockData);
// 		}
// 		
			
	
// 		for (auto* testActor : actor->childsActors)
// 		{
// 			APSH_BlockActor* TestchildsActors = Cast<APSH_BlockActor>(testActor);
// 			if (TestchildsActors->childsActors.IsEmpty())
// 			{
// 				UE_LOG(LogTemp, Warning, TEXT("ChildsActor : %s"), *TestchildsActors->GetName());
// 			}
// 			else
// 			{
// 				UE_LOG(LogTemp, Warning, TEXT("ChildsActor : %s"), *TestchildsActors->GetName());
// 				for (int i=0; i <TestchildsActors->childsActors.Num(); i++)
// 				{
// 					UE_LOG(LogTemp, Warning, TEXT("TestchildsActors : %s"), *TestchildsActors->childsActors[i]->GetName());
// 				}
// 			}
// 			
// 		}

// 		if (actor == nullptr) return;
// 
// 		FPSH_ObjectData data = actor->SaveBlockHierachy(); // 잡은 블럭 자식, 본인 전체 다 save
// 
// 		FName rowName = FName(*FString::FormatAsNumber(4));
// 
// 		dataTable->AddRow(rowName, data);
	
}
void APSH_Player::LoadTest()
{
	
	FName Rowname = FName(*FString::FormatAsNumber(4));
	FPSH_ObjectData* data = dataTable->FindRow<FPSH_ObjectData>(Rowname, TEXT("non"));
	float sum = 200.0f;
	APSH_BlockActor * sapwnPartent = nullptr;

	if (data && data->actor != nullptr)
	{
		// 루트 블럭 소환
		TSubclassOf<APSH_BlockActor> SpawnActor = data->actor;
		if (SpawnActor)
		{
			FActorSpawnParameters Params;
			APSH_BlockActor* SpawnedBlock = GetWorld()->SpawnActor<APSH_BlockActor>(SpawnActor, GetActorForwardVector() * sum, GetActorRotation(), Params);

			// 블럭 계층 구조 불러오기
			if (SpawnedBlock)
			{
				SpawnedBlock->LoadBlockHierarchy(*data);
			}
		}
	}
}
void APSH_Player::ShowInterface()
{
	// UI가 이미 열려있다면 닫기
	if (CurrentObjectWidget)
	{
		CurrentObjectWidget->RemoveFromParent();
		CurrentObjectWidget = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("UI Widget closed"));
	}
	else // UI가 열려있지 않다면 열기
	{
		if (FirstSelect)
		{
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController)
			{
				CurrentObjectWidget = CreateWidget<UUserWidget>(PlayerController, FirstSelect);
				if (CurrentObjectWidget)
				{
					CurrentObjectWidget->AddToViewport();
					UE_LOG(LogTemp, Warning, TEXT("UI Widget opened"));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to create UI widget"));
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FirstSelect widget class is not set"));
		}
	}
}

void APSH_Player::HorizontalRotChange(const FInputActionValue& value)
{
	float input2D = value.Get<float>();

	if (input2D > 0) // q
	{
		rotationOffset.Yaw = UKismetMathLibrary::ClampAxis(rotationOffset.Yaw + -45.f);
	}
	else
	{
		rotationOffset.Yaw = UKismetMathLibrary::ClampAxis(rotationOffset.Yaw + 45.f);
	}
}
void APSH_Player::VerticalRotChange(const FInputActionValue& value)
{
	float input2D = value.Get<float>();

	if (input2D > 0) // f
	{
		rotationOffset.Pitch = UKismetMathLibrary::ClampAxis(rotationOffset.Pitch + -45.f);
		//snapPointIndex = ((snapPointIndex - 1) % snapPointIndexLength + snapPointIndexLength) & snapPointIndexLength;
	}
	else // g
	{
		rotationOffset.Pitch = UKismetMathLibrary::ClampAxis(rotationOffset.Pitch + 45.f);
		//snapPointIndex = (snapPointIndex + 1) % snapPointIndexLength;
	}
}

void APSH_Player::BotMoveAndModeChange()
{
	if (bArtKey) // art 눌림
	{
		FHitResult hitresult;
		bool hit = pc->GetHitResultUnderCursor(ECC_Camera, false, hitresult);

		if (hit)
		{
			if (bot)
			{
				UE_LOG(LogTemp, Warning, TEXT("Move"));
				bot->MoveToLocation(*this,hitresult.ImpactPoint);
			}
		}
	}
	else // art 안눌림
	{
		FHitResult hitresult;
		bool hit = pc->GetHitResultUnderCursor(ECC_Camera, false, hitresult);

		if (hit)
		{
			bot = Cast<APSH_GarbageBot>(hitresult.GetActor());
			if(bot)
			{
				if (!botWidget->IsVisible())
				{
					botWidget->SetVisibility(ESlateVisibility::Visible);
					botWidget->SetOwner(bot);
				}
			}
		}
		// 오브젝트 선택했을 때 컴포넌트가 있으면 UI열리기
		AActor* SelectedActor = hitresult.GetActor();
		if (SelectedActor && (SelectedActor->FindComponentByClass<UMyMoveActorComponent>() || SelectedActor->FindComponentByClass<UMyFlyActorComponent>()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Calling SelectObject with Actor: %s"), *SelectedActor->GetName());
			// 플레이어 컨트롤러의 selectobject 함수 호출해서 UI 열기
			pc->SelectObject(SelectedActor);
		}
		else
		{
			// 선택된 오브젝트가 없거나 특정 컴포넌트가 없는경우 UI 닫기
			if (pc->MyChoiceActionWidget)
			{
				pc->MyChoiceActionWidget->RemoveFromParent();
				pc->MyChoiceActionWidget = nullptr;
			}
		}
	}
}

void APSH_Player::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	
}

void APSH_Player::OnArtKey()
{
	if (bArtKey)
	{
		bArtKey = false;
		UE_LOG(LogTemp,Warning,TEXT("Off"));
	}
	else
	{
		bArtKey = true;
		UE_LOG(LogTemp, Warning, TEXT("On"));
	}
	
}