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
#include "GameFramework/CharacterMovementComponent.h"
#include "PSH/PSH_Component/PSH_PlayerAnim.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraSystem.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "PSH/PSH_Actor/PSH_SpawnBot.h"

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

// 	previewMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("previewMesh"));
// 	previewMeshComp->SetupAttachment(RootComponent);

	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void APSH_Player::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		PRINTLOG(TEXT("Spawn"));
		FActorSpawnParameters param;
		APSH_SpawnBot * spawn = GetWorld()->SpawnActor<APSH_SpawnBot>(param);
		APSH_GarbageBot * garbage = GetWorld()->SpawnActor<APSH_GarbageBot>(param);

		if (spawn && garbage)
		{
			spawn->SetOwner(this);
			garbage->SetOwner(this);
			CRPC_SetBot(spawn, garbage);
		}
	}

	// ���콺 ���� ��� 
	if (IsLocallyControlled())
	{
		pc = Cast<APSH_PlayerController>(GetController());
		mouseWidget = Cast<UPSH_MouseWidget>(CreateWidget(GetWorld(), mouseWidgetFac));
		botWidget = Cast<UPSH_GarbageBotWidget>(CreateWidget(GetWorld(), botWidgetFac));
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

	anim = Cast<UPSH_PlayerAnim>(GetMesh()->GetAnimInstance());

}

// Called every frame
void APSH_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// ��ǥ ���̸� ���� (300 �Ǵ� 500)
	float TargetLength = bShouldExtend ? 500.0f : 300.0f;

	// ���� SpringArm�� ���̸� ��ǥ ���̷� �ε巴�� ����
	springArm->TargetArmLength = FMath::FInterpTo(springArm->TargetArmLength, TargetLength, DeltaTime, 3.0f); // 3.0f�� ���� �ӵ� ����

	if (GrabbedActor && pc != nullptr) // ��ü�� ���� �������� Ȯ��
	{
		FVector PlayerLocation = GetActorLocation();
		FVector ObjectLocation = GrabbedActor->GetActorLocation();

		// �÷��̾ ��ü�� �ٶ󺸵��� ȸ�� ���
		FRotator TargetRotation = FRotationMatrix::MakeFromX(ObjectLocation - PlayerLocation).Rotator();

		// �ε巴�� ȸ���ϵ��� InterpTo ����
		FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 5.0f);
		SetActorRotation(NewRotation);

		// ȭ�� ȸ��: ControlRotation�� Yaw�� ��� ȸ���� Yaw�� ���߾� �ε巴�� ȸ��
		FRotator CurrentControlRotation = GetControlRotation();

		// Yaw�� 360���� ���α� ���� DeltaYaw ���
		float DeltaYaw = FMath::UnwindDegrees(TargetRotation.Yaw - CurrentControlRotation.Yaw);

		if (FMath::Abs(DeltaYaw) > 60.0f) // ���� �̻� ���̳� ��� ȸ�� ����
		{
			if(FMath::Abs(DeltaYaw) > 90.0f) return;
			CurrentControlRotation.Yaw = FMath::FInterpTo(CurrentControlRotation.Yaw, CurrentControlRotation.Yaw + DeltaYaw, DeltaTime, 0.5f); // �ε巯�� ȸ��
			pc->SetControlRotation(CurrentControlRotation);
		}
	}

	if (pc && pc->IsLocalController())
	{
		// ������ ���ٸ� 
		if(handleComp->GetGrabbedComponent() == nullptr) return;

		// ������ ���� �ִٸ�
		if (handleComp->GetGrabbedComponent()->GetOwner() != nullptr)
		{
			FVector EffectEndLoc = handleComp->GetGrabbedComponent()->GetOwner()->GetActorLocation();
			SRPC_PickEffect(EffectEndLoc);
			//PRINTLOG(TEXT("Sever?"));
			auto* snap = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner());
			if(snap)
			snapPointIndexLength = snap->GetSnapPoints().Num();
		}

		FLocationPoint point; 
		PreTraceCheck(point.startLocation, point.endLocation); // 206 �� 58 �� ��

		ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);

		FHitResult hitinfo;

		bool hit = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			point.startLocation,
			point.endLocation,
			TraceChannel,
			false,
			actorsToIgnore,
			EDrawDebugTrace::None,
			hitinfo,
			true,
			FColor::Green,
			FColor::Red,
			4
		);

		SRPC_HandleBlock(hitinfo, hit, point.endLocation);
// 		FVector currentLocation = handleComp->GetGrabbedComponent()->GetOwner()->GetActorLocation();
// 		if (FVector::Dist(previousLocation, currentLocation) > PositionThreshold || timeSinceLastSync >= syncInterval)
// 		{
// 			
// 			previousLocation = currentLocation;
// 			timeSinceLastSync = 0.0f;  // ������ ����ȭ �� �ð� �ʱ�ȭ
// 		}
// 		else
// 		{
// 			timeSinceLastSync += DeltaTime;
// 		}
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
		// ������ ���̺� ���� ��
		EnhancedInputComponent->BindAction(inputActions[6], ETriggerEvent::Started, this, &APSH_Player::LoadTest);

		// �������̽� ����
		EnhancedInputComponent->BindAction(inputActions[7], ETriggerEvent::Started, this, &APSH_Player::ShowInterface);

		// �� ��� 45�� ������
		EnhancedInputComponent->BindAction(inputActions[8], ETriggerEvent::Started, this, &APSH_Player::HorizontalRotChange);
		
		// �� �Ʒ��� 45�� ������
		EnhancedInputComponent->BindAction(inputActions[9], ETriggerEvent::Started, this, &APSH_Player::VerticalRotChange);
		
		// ArtȮ��
		EnhancedInputComponent->BindAction(inputActions[10], ETriggerEvent::Started, this, &APSH_Player::OnArtKey);
		EnhancedInputComponent->BindAction(inputActions[10], ETriggerEvent::Completed, this, &APSH_Player::OnArtKey);

		// bot ���� �̵� ����
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
		
		PlaceBlock(OutHit, hit); // ��Ƽ , ����ȭ �ȵǾ� ����.
	}
	else // ������ ���ٸ�
	{
		CastRay();
	}
}

FVector APSH_Player::GetMouseDir()
{

	FVector WorldLoc;
	FVector WorldDir;

	if (pc == nullptr) return WorldDir;

	pc->DeprojectMousePositionToWorld(WorldLoc, WorldDir); // ���콺 ��ǥ ��ȯ

	return WorldDir;
}

void APSH_Player::CastRay() // ������� ���� 
{

	FVector worldDir = GetMouseDir();

	FVector StartLoc = cameraComp->GetComponentLocation();
	FVector EndLoc = StartLoc + (rayPower * worldDir);

	if (IsLocallyControlled() == false) return;

	SRPC_Pickup(StartLoc, EndLoc);
}

void APSH_Player::SRPC_Pickup_Implementation(const FVector& startLoc, const FVector& endLoc)
{
	// Ŭ��� ���� �������� �Ҹ��� �Լ�.
	FHitResult hitInfo;

	FCollisionQueryParams prams;
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startLoc, endLoc, ECC_Visibility, prams);

	if (bHit)
	{
		APSH_BlockActor* target = Cast<APSH_BlockActor>(hitInfo.GetActor());
		if (target)
		{
			if (target->GetMaster() == nullptr)
			{
				target->SetMaster(this);
				target->PickUp(handleComp);
				GrabbedActor = target;
				MRPC_PickupAnim(target);
			}
		}
	}

}

void APSH_Player::SRPC_PickEffect_Implementation(FVector endLoc)
{
	NRPC_PickEffect(endLoc);
}
void APSH_Player::NRPC_PickEffect_Implementation(FVector endLoc)
{
	FVector start = GetMesh()->GetSocketLocation(FName("LaserPoint"));
	UNiagaraComponent* effect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), pickEffect, start);
	if (!effect) return;

	effect->SetVectorParameter("Beam Start Loc", start);
	effect->SetVectorParameter("Beam End Loc", endLoc);
}

void APSH_Player::MRPC_PickupAnim_Implementation(class APSH_BlockActor* target)
{
	if (target == nullptr) return;
	GrabbedActor = target;
	if (anim == nullptr) return;

	anim->PlayAnimPickUp();
}
void APSH_Player::PreTraceCheck( FVector& StartLoc,  FVector& EndLoc) // 
{
	
	if (handleComp->GetGrabbedComponent() == nullptr) return;

	APSH_BlockActor* ChildBlcak = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner()); // Array�� ����

	if(ChildBlcak == nullptr) return;

	actorsToIgnore = ChildBlcak->childsActors;
	actorsToIgnore.Add(handleComp->GetGrabbedComponent()->GetOwner());

	FVector WorldDir = GetMouseDir();
	
	FVector CameraLoc = cameraComp->GetComponentLocation();
	StartLoc = CameraLoc;
	EndLoc = CameraLoc + playerReach * WorldDir;

}

void APSH_Player::SRPC_HandleBlock_Implementation(FHitResult hitinfo, bool hit, FVector endLoc)
{

	if (handleComp->GetGrabbedComponent() == nullptr) return;

	snapPointIndexLength = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner())->GetSnapPoints().Num();

	// �� �����Ӹ��� ������Ʈ�� ��ǥ ��ġ ���
	FVector targetLoc;
	FRotator targetRot;
	if (hit)
	{
		FVector localLocation = hitinfo.Location;
		FVector localNormal = hitinfo.Normal;

		auto* hitActor = Cast<APSH_BlockActor>(hitinfo.GetActor()); // ray�� �浹�� ����
		FTransform hitActorTransform = hitinfo.GetActor()->GetActorTransform();

		TArray<FVector> snapPoints; // ����Ʈ ��ġ��
		TArray<FRotator> snapDirection; // ����Ʈ ����

		FVector closestPoint = FVector::ZeroVector;
		float distance = 0;
		int32 ClosestSnapPointIndex = 0;

		auto* heldActor = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner()); // ���� ����
		if (Cast<APSH_BlockActor>(hitActor)) // ���� �ȿ� �ִ� ����
		{
			if (hitActor->GetSnapPoints().IsEmpty()) return; // ����ִٸ�

			snapPoints = hitActor->GetSnapPoints();
			ClosestPoint(snapPoints, localLocation, hitActorTransform, closestPoint, distance, ClosestSnapPointIndex);
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
			targetLoc = localLocation + (heldActor->GetActorLocation() -
				UKismetMathLibrary::TransformLocation(heldActor->GetActorTransform(), snapPoints[snapPointIndex]));
			targetRot = UKismetMathLibrary::MakeRotFromZ(localNormal);
		}
		else
		{
			targetLoc = UKismetMathLibrary::TransformLocation(hitActorTransform, closestPoint) +
				(heldActor->GetActorLocation() - UKismetMathLibrary::TransformLocation(heldActor->GetActorTransform(), snapPoints[snapPointIndex]));
			targetRot = UKismetMathLibrary::TransformRotation(hitActorTransform, snapDirection[ClosestSnapPointIndex]);
		}

		rotationHelper->SetWorldLocation(targetLoc);
		rotationHelper->SetWorldRotation(targetRot);
		rotationHelper->AddLocalRotation(WorldHelperRotationOffset());

		FRotator A = UKismetMathLibrary::RotatorFromAxisAndAngle(UKismetMathLibrary::GetRightVector(targetRot), rotationOffset.Pitch);
		FRotator B = UKismetMathLibrary::RotatorFromAxisAndAngle(UKismetMathLibrary::GetUpVector(targetRot), rotationOffset.Yaw);

		rotationHelper->AddWorldRotation(UKismetMathLibrary::ComposeRotators(A, B));
	}
	else
	{
		FVector newLoc = FMath::VInterpTo(rotationHelper->GetComponentLocation(), endLoc, GetWorld()->DeltaTimeSeconds, 5.0f); // �ӵ��� ���� ������ �̵�
		rotationHelper->SetWorldLocation(newLoc);

		// �ε巯�� �������� ��ǥ ȸ������ �̵�
		FRotator newRot = FMath::RInterpTo(rotationHelper->GetComponentRotation(), rotationOffset, GetWorld()->DeltaTimeSeconds, 5.0f);
		rotationHelper->SetWorldRotation(newRot);
	}

	MRPC_HandleBlock(rotationHelper->GetComponentLocation(), rotationHelper->GetComponentRotation());
}
FRotator APSH_Player::WorldHelperRotationOffset()
{
	TArray<FRotator> snapDir = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner())->GetSnapDirections();

	return UKismetMathLibrary::ComposeRotators(UKismetMathLibrary::NegateRotator(snapDir[snapPointIndex]), FRotator(180, 180, 0));

}
void APSH_Player::MRPC_HandleBlock_Implementation(FVector newLoc, FRotator newRot)
{
	handleComp->SetTargetLocationAndRotation(newLoc, newRot);
}


float APSH_Player::NormalizeAxis(float Angle)
{
	while (Angle > 180.0f) Angle -= 360.0f;
	while (Angle < -180.0f) Angle += 360.0f;
	return Angle;
}

void APSH_Player::ClosestPoint(TArray<FVector> pointArray, FVector testLocation, FTransform hitActorTransfrom ,
								FVector & closestPt, float& dist , int32& closetPointIndex)
{
	if (pointArray.Num() == 0) return;

	FVector testLoc = UKismetMathLibrary::InverseTransformLocation(hitActorTransfrom,testLocation);

	closestPt = pointArray[0]; //

	dist = FVector::Dist(closestPt, testLoc); //

	closetPointIndex = 0; //
		
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

void APSH_Player::PlaceBlock(FHitResult hitInfo, bool hit)
{
	SRPC_PlaceBlock(hitInfo,hit);
}	

void APSH_Player::SRPC_PlaceBlock_Implementation(FHitResult hitInfo, bool hit)
{
	APSH_BlockActor* heldBlock = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner());
	
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
					PRINTLOG(TEXT("PlaceAndDrop"));
				}
				else
				{
					PRINTLOG(TEXT("Not heldBlock->OvelapChek()"));
					return;
				}
			}
			else
			{
				DropBlcok();
				PRINTLOG(TEXT("DropBlock"));
			}
		}
		else
		{
			if (heldBlock->OvelapChek())
			{
				DropBlcok();
				PRINTLOG(TEXT("DropBlock Not Cast"));
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
			PRINTLOG(TEXT("DropBlock Not Hit"));
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
	PRINTLOG(TEXT("Player Drop"));
	Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner())->Drop(handleComp);
	MRPC_DropBlcok();
	GrabbedActor = nullptr;
}

void APSH_Player::MRPC_DropBlcok_Implementation()
{
	PRINTLOG(TEXT("Net Player Drop"));
	GrabbedActor = nullptr;
	if(anim == nullptr) return;
	anim->PlayAnimDrop();
}
void APSH_Player::SRPC_DropBlcok_Implementation()
{
	
}

void APSH_Player::ToggleARmLength()
{
	bShouldExtend = !bShouldExtend;
}

void APSH_Player::CRPC_SetBot_Implementation(class APSH_SpawnBot* spawn, class APSH_GarbageBot* garbage)
{
	if(spawn == nullptr || garbage == nullptr) return;

	spawnBot = spawn;
	spawnBot->SetPlayer(this);
	garbagebot = garbage;
	
}
void APSH_Player::NRPC_SetBot_Implementation(class APSH_SpawnBot* spawn, class APSH_GarbageBot* garbage)
{
	if (spawn == nullptr || garbage == nullptr) return;

	spawnBot = spawn;
	garbagebot = garbage;
	
}
void APSH_Player::SRPC_SpawnBlock_Implementation(TSubclassOf<class APSH_BlockActor> spawnActor)
{
	if(bSpawn == false) return;
	// ĳ������ ��ġ�� ������ �������� ����� ��ġ�� ����
	FVector CharacterLocation = GetActorLocation();
	FVector upVector = GetActorUpVector();  // ĳ������ ������ ����
	FVector ForwardVector = GetActorForwardVector();  // ĳ������ ���� ����

	// ����� ������ ���� (���������� 200, �������� �ణ ������ ��ġ)
	FVector SpawnLocation = CharacterLocation + (upVector * 300) + (ForwardVector * 300);

	// ���� �Ķ���� ���� �� ���� ��ȯ
	FActorSpawnParameters SpawnParams;
	APSH_BlockActor* SpawnActor = GetWorld()->SpawnActor<APSH_BlockActor>(spawnActor, SpawnLocation, GetActorRotation(), SpawnParams);
	SpawnActor->SetOwner(this);
}

void APSH_Player::LoadTest()
{
	
	FName Rowname = FName(*FString::FormatAsNumber(4));
	FPSH_ObjectData* data = dataTable->FindRow<FPSH_ObjectData>(Rowname, TEXT("non"));
	float sum = 200.0f;
	APSH_BlockActor * sapwnPartent = nullptr;

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
void APSH_Player::ShowInterface()
{
	// UI�� �̹� �����ִٸ� �ݱ�
	if (CurrentObjectWidget)
	{
		CurrentObjectWidget->RemoveFromParent();
		CurrentObjectWidget = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("UI Widget closed"));
	}
	else // UI�� �������� �ʴٸ� ����
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
	
	SRPC_HorizontalRotChange(value);
	
}
void APSH_Player::SRPC_HorizontalRotChange_Implementation(const FInputActionValue& value)
{
	NRPC_HorizontalRotChange(value);
}
void APSH_Player::NRPC_HorizontalRotChange_Implementation(const FInputActionValue& value)
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
	SRPC_VerticalRotChange(value);
}

void APSH_Player::SRPC_VerticalRotChange_Implementation(const FInputActionValue& value)
{
	NRPC_VerticalRotChange(value);
}
void APSH_Player::NRPC_VerticalRotChange_Implementation(const FInputActionValue& value)
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
	if (bArtKey) // art ����
	{
		FHitResult hitresult;
		bool hit = pc->GetHitResultUnderCursor(ECC_Camera, false, hitresult);

		if (hit)
		{
			if (garbagebot)
			{
				garbagebot->MoveToLocation(*this,hitresult.ImpactPoint);
			}
		}
	}
	else // art �ȴ���
	{
		FHitResult hitresult;
		bool hit = pc->GetHitResultUnderCursor(ECC_Camera, false, hitresult);

		if (hit)
		{
			garbagebot = Cast<APSH_GarbageBot>(hitresult.GetActor());
			if(garbagebot)
			{
				if (!botWidget->IsVisible())
				{
					botWidget->SetVisibility(ESlateVisibility::Visible);
					botWidget->SetOwner(garbagebot);
				}
			}
		}
		// ������Ʈ �������� �� ������Ʈ�� ������ UI������
		AActor* SelectedActor = hitresult.GetActor();
		if (SelectedActor && (SelectedActor->FindComponentByClass<UMyMoveActorComponent>() || SelectedActor->FindComponentByClass<UMyFlyActorComponent>()))
		{
			pc->SelectObject(SelectedActor);
		}
		else
		{
			// ���õ� ������Ʈ�� ���ų� Ư�� ������Ʈ�� ���°�� UI �ݱ�
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