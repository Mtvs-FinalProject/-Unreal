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
#include "PSH/PSH_UI/PSH_ObjectWidget.h"
#include "PSH/PSH_GameMode/PSH_GameModeBase.h"

// Sets default values
APSH_Player::APSH_Player()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/YWK/CH_AN/Character/rovotfbxligging.rovotfbxligging'"));

	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}

	ConstructorHelpers::FClassFinder<UAnimInstance> tempAnim(TEXT("/Script/Engine.AnimBlueprint'/Game/YWK/CH_AN/Animation/ABP_Player.ABP_Player_C'"));
	
	if (tempAnim.Succeeded())
	{
		GetMesh()->SetAnimClass(tempAnim.Class);
	}

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


	movementComp = GetCharacterMovement();

	if (movementComp)
	{
		movementComp->GetNavAgentPropertiesRef().bCanCrouch = true;
		movementComp->MaxFlySpeed = 500.0f;  // ���� �ִ� �ӵ� ����
		movementComp->BrakingDecelerationFlying = 2000.0f;  // ���ӵ� ����
	}

	



	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void APSH_Player::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{	
		FActorSpawnParameters param;
		garbagebot = GetWorld()->SpawnActor<APSH_GarbageBot>(param);
		if (garbagebot)
		{
			garbagebot->SetOwner(this);
			garbagebot->SetPlayer(this);
			garbagebot->InitializeMovePoint(); 
		}

		spawnBot = GetWorld()->SpawnActor<APSH_SpawnBot>(param);
		if (spawnBot)
		{
			spawnBot->SetOwner(this);
			spawnBot->SetPlayer(this);
		}

	}

	// ���콺 ���� ��� 
	if (IsLocallyControlled())
	{
		pc = Cast<APSH_PlayerController>(GetController());
		SRPC_CheckMode();
		InitPcUi();
		SRPC_Delegate();
	}
	
	anim = Cast<UPSH_PlayerAnim>(GetMesh()->GetAnimInstance());

}

// Called every frame
void APSH_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// ��ǥ ���̸� ���� (300 �Ǵ� 500)

	if (bFlyTimer)
	{
		curTime += DeltaTime;
		if (curTime > flyTime)
		{
			bFlyTimer = false;
			curTime = 0;
		}
	}

	float TargetLength = bShouldExtend ? 500.0f : 300.0f;

	// ���� SpringArm�� ���̸� ��ǥ ���̷� �ε巴�� ����
	springArm->TargetArmLength = FMath::FInterpTo(springArm->TargetArmLength, TargetLength, DeltaTime, 3.0f); // 3.0f�� ���� �ӵ� ����

	if (GrabbedActor && pc != nullptr) // ��ü�� ���� �������� Ȯ��
	{
		MoisePosition(DeltaTime);
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
			auto* snap = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner());
			if(snap)
			snapPointIndexLength = snap->GetSnapPoints().Num();
		}

		FVector StartLoc;
		FVector EndLoc;
		
		PreTraceCheck(StartLoc, EndLoc);

		ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);

		FHitResult hitinfo;

		bool hit = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			StartLoc,
			EndLoc,
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

		SRPC_HandleBlock(hitinfo, hit, EndLoc);
	}
	
}

void APSH_Player::InterpToRotation(const float & DeltaTime)
{
	FVector PlayerLocation = GetActorLocation();
	FVector ObjectLocation = GrabbedActor->GetActorLocation();
	// �÷��̾ ��ü�� �ٶ󺸵��� ȸ�� ���
	FRotator TargetRotation = FRotationMatrix::MakeFromX(ObjectLocation - PlayerLocation).Rotator();
	// �ε巴�� ȸ���ϵ��� InterpTo ����
	FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 5.0f);

	if (bFly)
	{
		NewRotation.Pitch = 0;
	}

	// ȭ�� ȸ��: ControlRotation�� Yaw�� ��� ȸ���� Yaw�� ���߾� �ε巴�� ȸ��
	SetActorRotation(NewRotation);

	FRotator CurrentControlRotation = GetControlRotation();
	// Yaw�� 360���� ���α� ���� DeltaYaw ���
	float DeltaYaw = FMath::UnwindDegrees(TargetRotation.Yaw - CurrentControlRotation.Yaw);
	if (FMath::Abs(DeltaYaw) > 60.0f && FMath::Abs(DeltaYaw) <= 90.0f) // ���� �̻� ���̳� ��� ȸ�� ����
	{
		CurrentControlRotation.Yaw = FMath::FInterpTo(CurrentControlRotation.Yaw, CurrentControlRotation.Yaw + DeltaYaw, DeltaTime, 0.5f); // �ε巯�� ȸ��
		pc->SetControlRotation(CurrentControlRotation);
	}
}


void APSH_Player::MoisePosition(const float& DeltaTime)
{
	if(pc == nullptr) return;

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	// ���콺 �̵� ���� ���� ����
	const float TopMargin = ViewportSize.Y * MarginPercent;  // ��ܿ��� 5%
	const float BottomMargin = ViewportSize.Y * (1.0f - MarginPercent);  // �ϴܿ��� 5%
	const float LeftMargin = ViewportSize.X * MarginPercent;  // �������� 5%
	const float RightMargin = ViewportSize.X * (1.0f - MarginPercent);  // �������� 5%

	// ĳ������ ȭ�� �߽� ��ġ ���
	FVector2D CharacterScreenPosition;
	if (pc && pc->ProjectWorldLocationToScreen(GetActorLocation(), CharacterScreenPosition))
	{
		// ���� ���콺 ��ġ Ȯ��
		float MouseX, MouseY;
		if (pc->GetMousePosition(MouseX, MouseY)) // veiwport �ȿ��� ���콺�� ��ġ�� Ȯ�� ������
		{
			// ĳ���Ϳ��� ���콺 ��ġ������ ���� (2D)
			FVector2D CharacterToMouse = FVector2D(MouseX, MouseY) - CharacterScreenPosition;

			InterpToRotation(DeltaTime);

			// ĳ���� ȭ�� ��ġ�� ���콺 ��ġ �� �Ÿ� ���
			float DistanceToCharacter = FVector2D::Distance(CharacterScreenPosition, FVector2D(MouseX, MouseY));

			bool needsRepositioning = false;
			FVector2D newMousePosition(MouseX, MouseY);

			// ĳ���Ϳ��� �ּ� �Ÿ� üũ
			if (DistanceToCharacter < MinDistance)
			{
				FVector2D DirectionToCharacter = CharacterToMouse.GetSafeNormal();
				newMousePosition = CharacterScreenPosition + DirectionToCharacter * MinDistance;
				needsRepositioning = true;
			}

			// ȭ�� ��� üũ �� ���ѵ� �������� Ŭ����
			if (MouseX <= LeftMargin || MouseX >= RightMargin ||
				MouseY <= TopMargin || MouseY >= BottomMargin)
			{
				newMousePosition.X = FMath::Clamp(newMousePosition.X, LeftMargin, RightMargin);
				newMousePosition.Y = FMath::Clamp(newMousePosition.Y, TopMargin, BottomMargin);
				needsRepositioning = true;
			}

			// ���콺 ��ġ ������Ʈ�� �ʿ��� ��쿡�� ����
			if (needsRepositioning)
			{
				pc->SetMouseLocation(newMousePosition.X, newMousePosition.Y);
			}

			// ����� ǥ�� (���� �߿��� ���)
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow,
					FString::Printf(TEXT("Mouse Position: X=%.1f, Y=%.1f"), MouseX, MouseY));
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green,
					FString::Printf(TEXT("Bottom Margin: %.1f"), BottomMargin));
			}
		}
		else // GetMousePosition ���� ��
		{
			// ������ ��ġ ���
			FVector2D SafePosition = CharacterScreenPosition;
			// ���ѵ� ���� ���� Ŭ����
			SafePosition.X = FMath::Clamp(SafePosition.X, LeftMargin, RightMargin);
			SafePosition.Y = FMath::Clamp(SafePosition.Y, TopMargin, BottomMargin);

			// �ּ� �Ÿ� ������ ���� ������
			FVector2D Offset(MinDistance * 0.707f, MinDistance * 0.707f);
			SafePosition += Offset;

			// �ٽ� �ѹ� ���ѵ� �������� Ŭ����
			SafePosition.X = FMath::Clamp(SafePosition.X, LeftMargin, RightMargin);
			SafePosition.Y = FMath::Clamp(SafePosition.Y, TopMargin, BottomMargin);

			pc->SetMouseLocation(SafePosition.X, SafePosition.Y);
		}
	}
}

void APSH_Player::DelegateTest()
{
	SRPC_ModeChangeDelegate();
}

// ��������Ʈ ��û
void APSH_Player::SRPC_ModeChangeDelegate_Implementation()
{
	APSH_GameModeBase * GM = Cast<APSH_GameModeBase>(GetWorld()->GetAuthGameMode());

	if (GM)
	{
		GM->MRPC_StartBlcok();
	}
}

// ��������Ʈ ���
void APSH_Player::SRPC_Delegate_Implementation()
{
	APSH_GameModeBase* GM = Cast<APSH_GameModeBase>(GetWorld()->GetAuthGameMode());

	if (GM)
	{
		GM->onStartBlock.AddDynamic(this, &APSH_Player::Delegatebool);
	}
}

// ��������Ʈ ��� �Լ�
void APSH_Player::Delegatebool(bool createMode)
{
	 bCreatingMode = createMode;

	 if (bCreatingMode == false) // �÷��̸��
	 {
		 bFly = false;

		 GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		 NRPC_SetUiVisible(bCreatingMode);
	 }
	 else
	 {
		 NRPC_SetUiVisible(bCreatingMode);
	 }
}


void APSH_Player::NRPC_SetUiVisible_Implementation(bool check)
{
	if (pc == nullptr) return;
	
	if (check)
	{
		if (pc->garbageBotWidget && pc->garbageBotWidget->IsVisible())
		{
			pc->garbageBotWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		if (pc->garbageBotWidget && pc->garbageBotWidget->IsVisible() == false)
		{
			pc->garbageBotWidget->SetVisibility(ESlateVisibility::Visible);
		}
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
		// �÷��̾� �ൿ
		EnhancedInputComponent->BindAction(inputActions[0], ETriggerEvent::Triggered, this, &APSH_Player::Move);
		EnhancedInputComponent->BindAction(inputActions[1], ETriggerEvent::Triggered, this, &APSH_Player::Look);
		EnhancedInputComponent->BindAction(inputActions[2], ETriggerEvent::Started, this, &APSH_Player::Grab);

		EnhancedInputComponent->BindAction(inputActions[3], ETriggerEvent::Started, this, &APSH_Player::PlayerJump);

		EnhancedInputComponent->BindAction(inputActions[4], ETriggerEvent::Started, this, &APSH_Player::PlayerCrouch);
		EnhancedInputComponent->BindAction(inputActions[4], ETriggerEvent::Completed, this, &APSH_Player::PlayerUnCrouch);

		EnhancedInputComponent->BindAction(inputActions[5], ETriggerEvent::Triggered, this, &APSH_Player::PlayerFly);
		// �ɱ׷� �ɱ�
		// \
		// �޸��� / �ȱ�
		
		// �������̽� ����
		EnhancedInputComponent->BindAction(inputActions[6], ETriggerEvent::Started, this, &APSH_Player::ShowInterface);

		// �� ��� 45�� ������
		EnhancedInputComponent->BindAction(inputActions[7], ETriggerEvent::Started, this, &APSH_Player::HorizontalRotChange);
		
		// �� �Ʒ��� 45�� ������
		EnhancedInputComponent->BindAction(inputActions[8], ETriggerEvent::Started, this, &APSH_Player::VerticalRotChange);
		
		// ArtȮ��
		EnhancedInputComponent->BindAction(inputActions[9], ETriggerEvent::Started, this, &APSH_Player::OnArtKey);
		EnhancedInputComponent->BindAction(inputActions[9], ETriggerEvent::Completed, this, &APSH_Player::OnArtKey);

		// bot ���� �̵� ����
		EnhancedInputComponent->BindAction(inputActions[10], ETriggerEvent::Started, this, &APSH_Player::BotMoveAndModeChange);

		// �� ũ�� ����
		EnhancedInputComponent->BindAction(inputActions[11], ETriggerEvent::Started, this, &APSH_Player::OnBlockScale);

		// ������ ���̺� ���� ��
		EnhancedInputComponent->BindAction(inputActions[12], ETriggerEvent::Started, this, &APSH_Player::LoadTest);
		EnhancedInputComponent->BindAction(inputActions[13], ETriggerEvent::Started, this, &APSH_Player::SaveTest);

		// ��� ����
		EnhancedInputComponent->BindAction(inputActions[14], ETriggerEvent::Started, this, &APSH_Player::DelegateTest);
		
	}
}

void APSH_Player::InitPcUi()
{
	if(pc == nullptr) return;
	
	if (pc->mouseWidgetFac)
	{
		pc->mouseWidget = Cast<UPSH_MouseWidget>(CreateWidget(GetWorld(), pc->mouseWidgetFac));
		if (pc->mouseWidget)
		{
			pc->SetMouseCursorWidget(EMouseCursor::Default, pc->mouseWidget);
		}
	}

	if (pc->garbageBotWidgetFac)
	{
		pc->garbageBotWidget = Cast<UPSH_GarbageBotWidget>(CreateWidget(GetWorld(), pc->garbageBotWidgetFac));
		if (pc->garbageBotWidget)
		{
			pc->garbageBotWidget->AddToViewport();
			if (!bCreatingMode)
			{
				pc->garbageBotWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}

	if (pc->objectWidgetFac)
	{
		pc->objectWidget = CreateWidget<UPSH_ObjectWidget>(GetWorld(), pc->objectWidgetFac);
		if (pc->objectWidget)
		{
			pc->objectWidget->AddToViewport();
			pc->objectWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

}

void APSH_Player::Move(const FInputActionValue& value)
{
	if (bFly) return; // ���� ����� ���� �۵�
	
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
	if (!bCreatingMode)
	{
		Jump();
	}
	else
	{
		if (bFly)
		{
			if (bFlyTimer)
			{
				bFly = false;
				PRINTLOG(TEXT("MOVE_Walking"));
				SRPC_SetMovementMode(MOVE_Walking);
				/*GetCharacterMovement()->SetMovementMode(MOVE_Walking);*/
			}
			else
			{
				bFlyTimer = true;
			}
		}
		else
		{
			if (bFlyTimer)
			{
				bFly = true;
				PRINTLOG(TEXT("MOVE_Flying"));
				SRPC_SetMovementMode(MOVE_Flying);
				//GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			}
			else
			{
				Jump();
				bFlyTimer = true;
			}
		}
	}
}

void APSH_Player::PlayerFly(const FInputActionValue& value)
{
	if(!bFly) return;
	FVector Value = value.Get<FVector>();

	FVector forwardVec = FRotationMatrix(pc->GetControlRotation()).GetUnitAxis(EAxis::X);
	FVector rightVec = FRotationMatrix(pc->GetControlRotation()).GetUnitAxis(EAxis::Y);
	FVector UpVector = FRotationMatrix(pc->GetControlRotation()).GetUnitAxis(EAxis::Z); // ��·� ����

	AddMovementInput(forwardVec, Value.X);
	AddMovementInput(rightVec, Value.Y);
	AddMovementInput(UpVector, Value.Z);
}

void APSH_Player::SRPC_SetMovementMode_Implementation(EMovementMode mode)
{
	GetCharacterMovement()->SetMovementMode(mode);
}
void APSH_Player::SRPC_CheckMode_Implementation()
{
	APSH_GameModeBase * gm = Cast<APSH_GameModeBase>(GetWorld()->GetAuthGameMode());

	if (gm)
	{
		MRPC_CheckMode(gm->GetCreateingCheck());
	}
}

void APSH_Player::MRPC_CheckMode_Implementation(bool check)
{
	bCreatingMode = check;
}

void APSH_Player::PlayerCrouch(const FInputActionValue& value)
{
	if (bFly) return;

	Crouch();
}

void APSH_Player::PlayerUnCrouch(const FInputActionValue& value)
{
	if (bFly) return;

	UnCrouch();
}

void APSH_Player::Grab()
{
	if (pc)
	{
		if (pc->garbageBotWidget)
		{
			if (pc->garbageBotWidget->bHoverdBot)
			{
				pc->garbageBotWidget->OnClickBackGround();
			}
		}
	}

	if (handleComp == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("NullHandle"))
			return;
	}

	if (handleComp->GetGrabbedComponent() != nullptr) // ���� �� �ִٸ�
	{
		FVector startLoc;
		FVector EndLoc;
		PreTraceCheck(startLoc, EndLoc);

		ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);
		FHitResult OutHit;

		bool hit = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			startLoc,
			EndLoc,
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
				target->SetOwner(this);
				target->SetOutLine(true);
				target->PickUp(handleComp);
				GrabbedActor = target;
				MRPC_PickupAnim(target);
			}
		}
	}

}

void APSH_Player::SRPC_PickEffect_Implementation(FVector endLoc)
{
	MRPC_PickEffect(endLoc);
}

void APSH_Player::MRPC_PickEffect_Implementation(FVector endLoc)
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
	PRINTLOG(TEXT("Anim"));
	anim->PlayAnimPickUp();
	GetCharacterMovement()->bOrientRotationToMovement = false;
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

	if (Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner())->GetSnapPoints().IsEmpty() == false)
	{
		snapPointIndexLength = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner())->GetSnapPoints().Num();
	}

	// �� �����Ӹ��� ������Ʈ�� ��ǥ ��ġ ���
	FVector targetLoc;
	FRotator targetRot;
	FTransform worldTransfrom;

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
			if (hitActor->GetSnapPoints().IsEmpty() == false) 
			{
				
			snapPoints = hitActor->GetSnapPoints();
			ClosestPoint(snapPoints, localLocation, hitActorTransform, closestPoint, distance, ClosestSnapPointIndex);
			snapDirection = hitActor->GetSnapDirections();
				/*Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner())->OvelapChek();*/
			}
		}

		bool distBool = distance <= snapDistance;
		bool bValidPoint = snapPoints.IsValidIndex(0);
		bool andChek = distBool && bValidPoint;

		if (heldActor == nullptr) return;
		
		if (heldActor->GetSnapPoints().IsEmpty() == false)
		{
			snapPoints = heldActor->GetSnapPoints();
		}

		if (!andChek)
		{
			if (snapPoints.IsEmpty()) // ����ٸ�
			{
				targetLoc = localLocation + (heldActor->GetActorLocation() - heldActor->GetActorTransform().GetLocation());
				targetRot = UKismetMathLibrary::MakeRotFromZ(localNormal);
			}
			else
			{
				targetLoc = localLocation + (heldActor->GetActorLocation() -
					UKismetMathLibrary::TransformLocation(heldActor->GetActorTransform(), snapPoints[snapPointIndex]));
				targetRot = UKismetMathLibrary::MakeRotFromZ(localNormal);
			}
		}
		else
		{
			if (snapPoints.IsEmpty()) // ����ٸ�
			{
				targetLoc = hitActorTransform.GetLocation() + heldActor->GetActorTransform().GetLocation();
				targetRot = hitActorTransform.GetRotation().Rotator();
			}
			else
			{
				targetLoc = UKismetMathLibrary::TransformLocation(hitActorTransform, closestPoint) +
					(heldActor->GetActorLocation() - UKismetMathLibrary::TransformLocation(heldActor->GetActorTransform(), 
					snapPoints[snapPointIndex]));
				targetRot = UKismetMathLibrary::TransformRotation(hitActorTransform, snapDirection[ClosestSnapPointIndex]);

			}
		}

		rotationHelper->SetWorldLocation(targetLoc);
		rotationHelper->SetWorldRotation(targetRot);

		if (Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner())->GetSnapDirections().IsEmpty() == false)
		{
			rotationHelper->AddLocalRotation(WorldHelperRotationOffset());
		}

		FRotator A = UKismetMathLibrary::RotatorFromAxisAndAngle(UKismetMathLibrary::GetRightVector(targetRot), rotationOffset.Pitch);
		FRotator B = UKismetMathLibrary::RotatorFromAxisAndAngle(UKismetMathLibrary::GetUpVector(targetRot), rotationOffset.Yaw);

		rotationHelper->AddWorldRotation(UKismetMathLibrary::ComposeRotators(A, B));

// 		if (Cast<APSH_BlockActor>(hitActor))
// 		{
// 			worldTransfrom = UKismetMathLibrary::MakeTransform(UKismetMathLibrary::InverseTransformLocation(
// 				heldActor->GetActorTransform(), targetLoc),
// 				rotationHelper->GetComponentRotation());
// 
// 			if (hitActor->connectionsize >= heldActor->connectionsize)
// 			{
// 				if (heldActor->OvelapChek())
// 				{
// 					heldActor->Place(hitActor, worldTransfrom);
// 					DropBlcok();
// 					snapPointIndex = 0;
// 				}
// 			}
// 			return;
// 		}
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

FRotator APSH_Player::WorldHelperRotationOffset() // �߰�����
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
								FVector& closestPt, float& dist , int32& closetPointIndex)
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
			FTransform worldTransfrom;
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
				if (snapPoints.IsEmpty())
				{
					PRINTLOG(TEXT("snapPoints.IsEmpty()"));
					snapLocation = location + (heldBlock->GetActorLocation() - heldBlock->GetActorTransform().GetLocation());
				}
				else
				{
					snapLocation = location + (heldBlock->GetActorLocation() -
						UKismetMathLibrary::TransformLocation(heldBlock->GetActorTransform(), heldBlockPoints[snapPointIndex]));
				}
			}
			else
			{
				if (snapPoints.IsEmpty())
				{
					snapLocation = actor->GetActorTransform().GetLocation() + (heldBlock->GetActorLocation() -
					heldBlock->GetActorTransform().GetLocation());
				}
				else
				{

					snapLocation = UKismetMathLibrary::TransformLocation(actor->GetActorTransform(), closestPoint) +
						(heldBlock->GetActorLocation() -
							UKismetMathLibrary::TransformLocation(heldBlock->GetActorTransform(), heldBlockPoints[snapPointIndex]));
				}
			}

			worldTransfrom = UKismetMathLibrary::MakeTransform(UKismetMathLibrary::InverseTransformLocation(
				actor->GetActorTransform(), snapLocation),
				rotationHelper->GetComponentRotation());

			if (actor->connectionsize >= heldBlock->connectionsize)
			{
				if (heldBlock->OvelapChek())
				{
					if (heldBlock->mapBlock && actor->mapBlock) // �� ������Ʈ �� �� ������Ʈ
					{
						PRINTLOG(TEXT("heldBlock->mapBlock && actor->mapBlock"));
						heldBlock->Place(actor, worldTransfrom);
						DropBlcok();
						snapPointIndex = 0;
					}
					else if(heldBlock->mapBlock || actor->mapBlock) // �� ������Ʈ�� �ƴ� �� �� ������Ʈ
					{
						DropBlcok();
						snapPointIndex = 0;
					}
					else
					{
						heldBlock->Place(actor, worldTransfrom);
						DropBlcok();
						snapPointIndex = 0;
					}
					
				}
				else
				{
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
			return;
		}
	}
}

void APSH_Player::DropBlcok()
{
	APSH_BlockActor * taget = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner());
	
	if(taget == nullptr) return;

	taget->Drop(handleComp);
	taget->SetOutLine(false);

	MRPC_DropBlcok();
	GrabbedActor = nullptr;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void APSH_Player::MRPC_DropBlcok_Implementation()
{
	GrabbedActor = nullptr;

	if(anim == nullptr) return;
	anim->PlayAnimDrop();
}

void APSH_Player::ToggleARmLength()
{
	bShouldExtend = !bShouldExtend;
}

void APSH_Player::SRPC_SpawnbotIdel_Implementation()
{
	if (spawnBot == nullptr) return;
	spawnBot->SetState(EspawnState::IDLEMOVE);
}

void APSH_Player::SRPC_SpawnBotMoveTo_Implementation()
{
	if(spawnBot == nullptr) return;

	spawnBot->MoveTo();

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

void APSH_Player::SRPC_GabageBotMovePoint_Implementation(const FVector& Movelocation)
{
	if (garbagebot)
	{
		garbagebot->SRPC_MoveToLocation(Movelocation);
	}
}
void APSH_Player::SRPC_GarbageBotInitPoint_Implementation()
{
	if (garbagebot == nullptr) return;

	if (garbagebot->compMesh->IsVisible() == false)
	{
		garbagebot->compMesh->SetVisibility(true);
	}

	garbagebot->InitializeMovePoint();
}

void APSH_Player::SRPC_GarbageBotSetState_Implementation(EState state)
{
	if (garbagebot == nullptr) return;

	garbagebot->SetState(state);
}

void APSH_Player::BotMoveAndModeChange()
{
	if (bArtKey) // art ����
	{
		FHitResult hitresult;
		bool hit = pc->GetHitResultUnderCursor(ECC_Camera, false, hitresult);

		if (hit)
		{
			SRPC_GabageBotMovePoint(hitresult.ImpactPoint);
		}
	}
	else // art �ȴ���
	{
		FHitResult hitresult;
		bool hit = pc->GetHitResultUnderCursor(ECC_Camera, false, hitresult);

		if (hit)
		{
			if (garbagebot)
			{
				if (!pc->garbageBotWidget->IsVisible())
				{
					pc->garbageBotWidget->SetVisibility(ESlateVisibility::Visible);
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

void APSH_Player::SaveTest()
{
	SRPC_Save();
}

void APSH_Player::LoadTest()
{
	SRPC_Load();
}

void APSH_Player::SRPC_Save_Implementation()
{
	TArray<AActor*> blockArray;
	// "owner" �±װ� �޸� ��� ����� ������
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APSH_BlockActor::StaticClass(), FName(TEXT("owner")), blockArray);

	for (AActor* arrayActor : blockArray)
	{
		APSH_BlockActor* blockActor = Cast<APSH_BlockActor>(arrayActor);

		if (blockActor)
		{
			FName rowName = FName(*FString::FormatAsNumber(RowNum++));

			// ���� ���� ����
			FPSH_ObjectData BlockData = blockActor->SaveBlockHierachy();

			// ������ ���̺� �߰�
			if (dataTable)
			{
				dataTable->AddRow(rowName, BlockData);
			}
		}
	}
}
void APSH_Player::SRPC_Load_Implementation()
{

	if (!dataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("DataTable is null!"));
		return;
	}
	if (!dataTable) return;


	// ������ ���̺��� ��� �� ��������
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
}

void APSH_Player::ShowInterface()
{
	if(!bCreatingMode) return;
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
	MRPC_HorizontalRotChange(value);
}

void APSH_Player::MRPC_HorizontalRotChange_Implementation(const FInputActionValue& value)
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
	MRPC_VerticalRotChange(value);
}

void APSH_Player::MRPC_VerticalRotChange_Implementation(const FInputActionValue& value)
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

void APSH_Player::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APSH_Player, bCreatingMode);
	DOREPLIFETIME(APSH_Player, bFly);
	DOREPLIFETIME(APSH_Player, bFlyTimer);
	
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
void APSH_Player::OnBlockScale(const FInputActionValue& value)
{
	if(GrabbedActor == nullptr) return;

	float axis = value.Get<float>();

	GrabbedActor->SRPC_BlockScale(axis);
	//ũ�� ���� +=
}
