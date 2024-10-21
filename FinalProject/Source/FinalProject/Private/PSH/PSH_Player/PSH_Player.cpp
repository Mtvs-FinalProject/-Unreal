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

// Sets default values
APSH_Player::APSH_Player()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ������ ������Ʈ
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	springArm->SetupAttachment(RootComponent);

	// ī�޶� ������Ʈ
	rotationHelper = CreateDefaultSubobject<USceneComponent>(TEXT("RotationHelper"));
	rotationHelper->SetupAttachment(RootComponent);

	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	cameraComp->SetupAttachment(springArm);

	// ������ �ڵ� ������Ʈ
	handleComp = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("Handle"));
}

// Called when the game starts or when spawned
void APSH_Player::BeginPlay()
{
	Super::BeginPlay();
	
	// ���콺 ���� ��� 
	mouseWidget = Cast<UPSH_MouseWidget>(CreateWidget(GetWorld(), mouseWidgetFac));

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
	

	if(handleComp->GetGrabbedComponent() == nullptr) return;

	if (handleComp->GetGrabbedComponent()->GetOwner() != nullptr)
	{
		auto* snap = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner());
		if(snap)
		snapPointIndexLength = snap->GetSnapPoints().Num();
	}

	FLocationPoint point; 
	PreTraceCheck(point.startLocation, point.endLocation); // 206 �� 58 �� ��
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

	HandleBlock(outHit,hit, point.endLocation);
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

		// ���� ��
		EnhancedInputComponent->BindAction(inputActions[4], ETriggerEvent::Started, this, &APSH_Player::SpawnBlock);

		// �� ����
		EnhancedInputComponent->BindAction(inputActions[5], ETriggerEvent::Started, this, &APSH_Player::SaveTest);

		// ������ ���̺� ���� ��
		EnhancedInputComponent->BindAction(inputActions[6], ETriggerEvent::Started, this, &APSH_Player::LoadTest);
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
void APSH_Player::Grab()
{
	if (handleComp == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("NullHandle"))
			return;
	}

	if (handleComp->GetGrabbedComponent() != nullptr) // ���� �� �ִٸ�
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

		PlaceBlock(OutHit,hit);
	}
	else // ������ ���ٸ�
	{
		
		FHitResult outHit;

		outHit = CastRay();

		APSH_BlockActor * target = Cast<APSH_BlockActor>(outHit.GetActor());
		if (target)
		{
			target->PickUp(handleComp);
		}
		
	}
}
void APSH_Player::PreTraceCheck(FVector& StartLoc, FVector& EndLoc) // ī�޶�� ���콺�� ��ġ�� �̿��� Ʈ���̽� �Ÿ� ���
{
	if (handleComp->GetGrabbedComponent() == nullptr) return;

	APSH_BlockActor* ChildBlcak = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner()); // Array�� ����

	if(ChildBlcak == nullptr) return;

	FVector WorldLoc;
	FVector WorldDir;

	pc->DeprojectMousePositionToWorld(WorldLoc, WorldDir); // ���콺 ��ǥ ��ȯ

	actorsToIgnore = ChildBlcak->childsActors;
	actorsToIgnore.Add(handleComp->GetGrabbedComponent()->GetOwner());
	
	FVector CameraLoc = cameraComp->GetComponentLocation();
	StartLoc = CameraLoc;
	EndLoc = CameraLoc + playerReach * WorldDir;

}

FHitResult APSH_Player::CastRay() // ������� ���� 
{
	FHitResult hitInfo;

	FVector worldLoc;
	FVector worldDir;
	pc->DeprojectMousePositionToWorld(worldLoc, worldDir);

	FVector StartLoc = cameraComp->GetComponentLocation();
	FVector EndLoc = StartLoc + (rayPower * worldDir);

	FCollisionQueryParams prams;
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, StartLoc, EndLoc, ECC_Visibility, prams);

// 	if (bHit)
// 	{
// 		DrawDebugSphere(GetWorld(), hitInfo.ImpactPoint, 10, 10, FColor::Red, false, 5);
// 	}

	return hitInfo;
}

void APSH_Player::ClosestPoint(TArray<FVector> pointArray, FVector testLocation, FTransform hitActorTransfrom ,
								FVector & closestPt, float& dist , int32& closetPointIndex)
{
	if (pointArray.Num() == 0) return;

	FVector testLoc = UKismetMathLibrary::InverseTransformLocation(hitActorTransfrom,testLocation);


	
		closestPt = pointArray[0]; //

		dist = FVector::Dist(closestPt, testLoc); //

		closetPointIndex = 0; //
		
	// ���� �ѹ��� �ݺ�
	
	for (int32 i = 0 ; i < pointArray.Num(); i++) // ���� ����� ����Ʈ ã��
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
void APSH_Player::PlaceBlock(FHitResult hitInfo, bool hit)
{
	APSH_BlockActor * heldBlock =  Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner());

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

			ClosestPoint(snapPoints,location,actor->GetActorTransform(), closestPoint, distance,arrayindex);

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
					heldBlock->Place(actor,worldTransfrom);
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
			if(heldBlock->OvelapChek())
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
	Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner())->Drop(handleComp);
}

void APSH_Player::HandleBlock(FHitResult hitinfo, bool hit, FVector rayEndLocation)
{
	if(handleComp->GetGrabbedComponent() == nullptr) return;

	snapPointIndexLength = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner())->GetSnapPoints().Num();
	if (hit)
	{
		FVector localLocation = hitinfo.Location;
		FVector localNormal = hitinfo.Normal;
		auto* hitActor = Cast<APSH_BlockActor>(hitinfo.GetActor()); // ray�� �浹�� ����
		FTransform hitActorTransfrom = hitinfo.GetActor()->GetActorTransform();

		TArray<FVector> snapPoints; // ����Ʈ ��ġ��
		TArray<FRotator> snapDirection; // ����Ʈ ����

		FVector closestPoint = FVector::ZeroVector; 
		float distance = 0;
		int32 ClosestSnapPointIndex = 0;

		FVector newLoc; // ���ο� ��ġ
		FRotator newRot; // ���ο� ����

		auto* heldActor = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner()); // ���� ����
		if (Cast<APSH_BlockActor>(hitActor)) // ���� �ȿ� �ִ� ����
		{
			if (hitActor->GetSnapPoints().IsEmpty()) return; // ����ִٸ�

			snapPoints = hitActor->GetSnapPoints();
			
			ClosestPoint(snapPoints, localLocation, hitActorTransfrom, closestPoint, distance, ClosestSnapPointIndex);
			snapDirection = hitActor->GetSnapDirections();
			Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner())->OvelapChek();
		}
		
		bool bDist = snapDistance >= distance; // ������
		bool bValidPoint = snapPoints.IsValidIndex(0); // ����
		bool andBool = bDist && bValidPoint && hit;

		if (heldActor == nullptr) return;

		snapPoints = heldActor->GetSnapPoints();

		if (!andBool)
		{
			newLoc = localLocation + (heldActor->GetActorLocation() -
				UKismetMathLibrary::TransformLocation(heldActor->GetActorTransform(), snapPoints[snapPointIndex]));
		}
		else // �̰� ������ �ȵ�.
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

		handleComp->SetTargetLocationAndRotation(rotationHelper->GetComponentLocation(), rotationHelper->GetComponentRotation());
	}
	else
	{
		handleComp->SetTargetLocationAndRotation(rayEndLocation,rotationOffset);
	}
}
void APSH_Player::SpawnBlock()
{
	for (int i = 1; i <= 3; i++)
	{
		FName Rowname = FName(*FString::FormatAsNumber(i));
		FPSH_ObjectData* data = dataTable->FindRow<FPSH_ObjectData>(Rowname, TEXT("non"));
		float sum = 200.0f;
		if (data && data->actor != nullptr)
		{
			TSubclassOf<APSH_BlockActor> spawnActor = data->actor;
			if (spawnActor != nullptr)
			{
				FActorSpawnParameters parm;
				GetWorld()->SpawnActor<APSH_BlockActor>(spawnActor, GetActorLocation() + (GetActorForwardVector() * sum), GetActorRotation(), parm);
				sum += 100.f;
			}
		}
	}
}
void APSH_Player::SaveTest()
{

		if (handleComp->GetGrabbedComponent()->GetOwner() == nullptr) return; 

		APSH_BlockActor* actor = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner()); // ���� ��
		FName rowName = FName(*FString::FormatAsNumber(4));
		if (actor && dataTable)
		{
			FPSH_ObjectData BlockData = actor->SaveBlockHierachy();
			dataTable->AddRow(rowName, BlockData);
		}
		
			
	
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
// 		FPSH_ObjectData data = actor->SaveBlockHierachy(); // ���� �� �ڽ�, ���� ��ü �� save
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

// 	if (sapwnPartent && data)
// 	{
// 		TSet<APSH_BlockActor*> ProcessedBlocks;
// 		sapwnPartent->LoadBlockHierarchy(*data);
// 	}
// 
// 	FName Rowname = FName(*FString::FormatAsNumber(rowNam));
// 	FPSH_ObjectData* Data = DataTable->FindRow<FPSH_ObjectData>(Rowname, TEXT("non"));

	if (data && data->actor != nullptr)
	{
		// ��Ʈ �� ��ȯ
		TSubclassOf<APSH_BlockActor> SpawnActor = data->actor;
		if (SpawnActor)
		{
			FActorSpawnParameters Params;
			APSH_BlockActor* SpawnedBlock = GetWorld()->SpawnActor<APSH_BlockActor>(SpawnActor, GetActorForwardVector() * sum, GetActorRotation(), Params);

			// �� ���� ���� �ҷ�����
			if (SpawnedBlock)
			{
				SpawnedBlock->LoadBlockHierarchy(*data);
			}
		}
	}
}
