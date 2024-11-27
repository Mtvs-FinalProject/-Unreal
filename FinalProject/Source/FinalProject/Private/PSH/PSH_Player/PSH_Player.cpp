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
#include "YWK/RotationWidget.h"
#include "JsonObjectConverter.h"

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


	movementComp = GetCharacterMovement();

	if (movementComp)
	{
		movementComp->GetNavAgentPropertiesRef().bCanCrouch = true;
		movementComp->MaxFlySpeed = 500.0f;  // 비행 최대 속도 설정
		movementComp->BrakingDecelerationFlying = 2000.0f;  // 감속도 설정
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
			//garbagebot->InitializeMovePoint(); 
		}

		spawnBot = GetWorld()->SpawnActor<APSH_SpawnBot>(param);
		if (spawnBot)
		{
			spawnBot->SetOwner(this);
			spawnBot->SetPlayer(this);
		}

	}

	// 마우스 위젯 사용 
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
	// 목표 길이를 설정 (300 또는 500)

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

	// 현재 SpringArm의 길이를 목표 길이로 부드럽게 보간
	springArm->TargetArmLength = FMath::FInterpTo(springArm->TargetArmLength, TargetLength, DeltaTime, 3.0f); // 3.0f는 보간 속도 조정

	if (GrabbedActor && pc != nullptr) // 물체를 잡은 상태인지 확인
	{
		MoisePosition(DeltaTime);
	}

	if (pc && pc->IsLocalController())
	{
		// 잡은게 없다면 
		
		if(handleComp->GetGrabbedComponent() == nullptr) return;

		// 잡은애 블럭이 있다면
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
	// 플레이어가 물체를 바라보도록 회전 계산
	FRotator TargetRotation = FRotationMatrix::MakeFromX(ObjectLocation - PlayerLocation).Rotator();
	// 부드럽게 회전하도록 InterpTo 적용
	FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 5.0f);

	if (bFly)
	{
		NewRotation.Pitch = 0;
	}

	// 화면 회전: ControlRotation의 Yaw를 대상 회전의 Yaw에 맞추어 부드럽게 회전
	SetActorRotation(NewRotation);

	FRotator CurrentControlRotation = GetControlRotation();
	// Yaw를 360도로 감싸기 위한 DeltaYaw 계산
	float DeltaYaw = FMath::UnwindDegrees(TargetRotation.Yaw - CurrentControlRotation.Yaw);
	if (FMath::Abs(DeltaYaw) > 60.0f && FMath::Abs(DeltaYaw) <= 90.0f) // 일정 이상 차이날 경우 회전 시작
	{
		CurrentControlRotation.Yaw = FMath::FInterpTo(CurrentControlRotation.Yaw, CurrentControlRotation.Yaw + DeltaYaw, DeltaTime, 0.5f); // 부드러운 회전
		pc->SetControlRotation(CurrentControlRotation);
	}
}


void APSH_Player::MoisePosition(const float& DeltaTime)
{
	if(pc == nullptr) return;

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	// 마우스 이동 제한 영역 설정
	const float TopMargin = ViewportSize.Y * MarginPercent;  // 상단에서 5%
	const float BottomMargin = ViewportSize.Y * (1.0f - MarginPercent);  // 하단에서 5%
	const float LeftMargin = ViewportSize.X * MarginPercent;  // 좌측에서 5%
	const float RightMargin = ViewportSize.X * (1.0f - MarginPercent);  // 우측에서 5%

	// 캐릭터의 화면 중심 위치 계산
	FVector2D CharacterScreenPosition;
	if (pc && pc->ProjectWorldLocationToScreen(GetActorLocation(), CharacterScreenPosition))
	{
		// 현재 마우스 위치 확인
		float MouseX, MouseY;
		if (pc->GetMousePosition(MouseX, MouseY)) // veiwport 안에서 마우스의 위치가 확인 됬을때
		{
			// 캐릭터에서 마우스 위치까지의 벡터 (2D)
			FVector2D CharacterToMouse = FVector2D(MouseX, MouseY) - CharacterScreenPosition;

			InterpToRotation(DeltaTime);

			// 캐릭터 화면 위치와 마우스 위치 간 거리 계산
			float DistanceToCharacter = FVector2D::Distance(CharacterScreenPosition, FVector2D(MouseX, MouseY));

			bool needsRepositioning = false;
			FVector2D newMousePosition(MouseX, MouseY);

			// 캐릭터와의 최소 거리 체크
			if (DistanceToCharacter < MinDistance)
			{
				FVector2D DirectionToCharacter = CharacterToMouse.GetSafeNormal();
				newMousePosition = CharacterScreenPosition + DirectionToCharacter * MinDistance;
				needsRepositioning = true;
			}

			// 화면 경계 체크 및 제한된 영역으로 클램핑
			if (MouseX <= LeftMargin || MouseX >= RightMargin ||
				MouseY <= TopMargin || MouseY >= BottomMargin)
			{
				newMousePosition.X = FMath::Clamp(newMousePosition.X, LeftMargin, RightMargin);
				newMousePosition.Y = FMath::Clamp(newMousePosition.Y, TopMargin, BottomMargin);
				needsRepositioning = true;
			}

			// 마우스 위치 업데이트가 필요한 경우에만 실행
			if (needsRepositioning)
			{
				pc->SetMouseLocation(newMousePosition.X, newMousePosition.Y);
			}

			// 디버그 표시 (개발 중에만 사용)
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow,
					FString::Printf(TEXT("Mouse Position: X=%.1f, Y=%.1f"), MouseX, MouseY));
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green,
					FString::Printf(TEXT("Bottom Margin: %.1f"), BottomMargin));
			}
		}
		else // GetMousePosition 실패 시
		{
			// 안전한 위치 계산
			FVector2D SafePosition = CharacterScreenPosition;
			// 제한된 영역 내로 클램핑
			SafePosition.X = FMath::Clamp(SafePosition.X, LeftMargin, RightMargin);
			SafePosition.Y = FMath::Clamp(SafePosition.Y, TopMargin, BottomMargin);

			// 최소 거리 유지를 위한 오프셋
			FVector2D Offset(MinDistance * 0.707f, MinDistance * 0.707f);
			SafePosition += Offset;

			// 다시 한번 제한된 영역으로 클램핑
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

// 델리게이트 요청
void APSH_Player::SRPC_ModeChangeDelegate_Implementation()
{
	APSH_GameModeBase * GM = Cast<APSH_GameModeBase>(GetWorld()->GetAuthGameMode());

	if (GM)
	{
		GM->MRPC_StartBlcok();
	}
}

// 델리게이트 등록
void APSH_Player::SRPC_Delegate_Implementation()
{
	APSH_GameModeBase* GM = Cast<APSH_GameModeBase>(GetWorld()->GetAuthGameMode());

	if (GM)
	{
		GM->onStartBlock.AddDynamic(this, &APSH_Player::Delegatebool);
	}
}

// 델리게이트 등록 함수
void APSH_Player::Delegatebool(bool createMode)
{
	 bCreatingMode = createMode;

	 if (bCreatingMode == false) // 플레이모드
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
		if (pc->garbageBotWidget)
		{
			PRINTLOG(TEXT("Hidden"));
			pc->garbageBotWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		if (pc->garbageBotWidget )
		{
			PRINTLOG(TEXT("Visible"));
			pc->garbageBotWidget->SetVisibility(ESlateVisibility::Hidden);
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
		// 플레이어 행동
		EnhancedInputComponent->BindAction(inputActions[0], ETriggerEvent::Triggered, this, &APSH_Player::Move);
		EnhancedInputComponent->BindAction(inputActions[1], ETriggerEvent::Triggered, this, &APSH_Player::Look);
		EnhancedInputComponent->BindAction(inputActions[2], ETriggerEvent::Started, this, &APSH_Player::Grab);

		EnhancedInputComponent->BindAction(inputActions[3], ETriggerEvent::Started, this, &APSH_Player::PlayerJump);

		EnhancedInputComponent->BindAction(inputActions[4], ETriggerEvent::Started, this, &APSH_Player::PlayerCrouch);
		EnhancedInputComponent->BindAction(inputActions[4], ETriggerEvent::Completed, this, &APSH_Player::PlayerUnCrouch);

		EnhancedInputComponent->BindAction(inputActions[5], ETriggerEvent::Triggered, this, &APSH_Player::PlayerFly);
		// 쪼그려 앉기
		// \
		// 달리기 / 걷기
		
		// 인터페이스 관련
		EnhancedInputComponent->BindAction(inputActions[6], ETriggerEvent::Started, this, &APSH_Player::ShowInterface);

		// 좌 우로 45도 돌리기
		EnhancedInputComponent->BindAction(inputActions[7], ETriggerEvent::Started, this, &APSH_Player::HorizontalRotChange);
		
		// 위 아래로 45도 돌리기
		EnhancedInputComponent->BindAction(inputActions[8], ETriggerEvent::Started, this, &APSH_Player::VerticalRotChange);
		
		// Art확인
		EnhancedInputComponent->BindAction(inputActions[9], ETriggerEvent::Started, this, &APSH_Player::OnArtKey);
		EnhancedInputComponent->BindAction(inputActions[9], ETriggerEvent::Completed, this, &APSH_Player::OnArtKey);

		// bot 모드와 이동 관리
		EnhancedInputComponent->BindAction(inputActions[10], ETriggerEvent::Started, this, &APSH_Player::BotMoveAndModeChange);

		// 블럭 크기 조정
		EnhancedInputComponent->BindAction(inputActions[11], ETriggerEvent::Started, this, &APSH_Player::OnBlockScale);

		// 데이터 테이블 스폰 블럭
		EnhancedInputComponent->BindAction(inputActions[12], ETriggerEvent::Started, this, &APSH_Player::LoadTest);
		EnhancedInputComponent->BindAction(inputActions[13], ETriggerEvent::Started, this, &APSH_Player::SaveTest);

		// 모드 변경
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
			pc->garbageBotWidget->SetVisibility(ESlateVisibility::Hidden);
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

	if (pc->MyChoiceActionWidgetFac)
	{
		pc->MyChoiceActionWidget = CreateWidget<UMyChoiceActionWidget>(GetWorld(), pc->MyChoiceActionWidgetFac);
		if (pc->MyChoiceActionWidget)
		{
			pc->MyChoiceActionWidget->AddToViewport();
			pc->MyChoiceActionWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

}

void APSH_Player::Move(const FInputActionValue& value)
{
	if (bFly) return; // 비행 모드일 때만 작동
	
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
	FVector UpVector = FRotationMatrix(pc->GetControlRotation()).GetUnitAxis(EAxis::Z); // 상승력 증가

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

	if (handleComp->GetGrabbedComponent() != nullptr) // 잡은 게 있다면
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
		
		PlaceBlock(OutHit, hit); // 멀티 , 서버화 안되어 있음.
	}
	else // 잡은게 없다면
	{
		CastRay();
	}
}

FVector APSH_Player::GetMouseDir()
{
	FVector WorldLoc;
	FVector WorldDir;

	if (pc == nullptr) return WorldDir;

	pc->DeprojectMousePositionToWorld(WorldLoc, WorldDir); // 마우스 좌표 변환

	return WorldDir;
}

void APSH_Player::CastRay() // 잡기위한 레이 
{
	FVector worldDir = GetMouseDir();

	FVector StartLoc = cameraComp->GetComponentLocation();
	FVector EndLoc = StartLoc + (rayPower * worldDir);

	if (IsLocallyControlled() == false) return;

	SRPC_Pickup(StartLoc, EndLoc);
}

void APSH_Player::SRPC_Pickup_Implementation(const FVector& startLoc, const FVector& endLoc)
{
	// 클라든 어디든 서버에서 불리는 함수.
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
	anim->PlayAnimPickUp();
	GetCharacterMovement()->bOrientRotationToMovement = false;
}
void APSH_Player::PreTraceCheck( FVector& StartLoc,  FVector& EndLoc) // 
{
	if (handleComp->GetGrabbedComponent() == nullptr) return;

	APSH_BlockActor* ChildBlcak = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner()); // Array에 저장

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

	// 매 프레임마다 업데이트된 목표 위치 계산
	FVector targetLoc;
	FRotator targetRot;
	FTransform worldTransfrom;

	if (hit)
	{
		FVector localLocation = hitinfo.Location;
		FVector localNormal = hitinfo.Normal;

		auto* hitActor = Cast<APSH_BlockActor>(hitinfo.GetActor()); // ray에 충돌한 엑터
		FTransform hitActorTransform = hitinfo.GetActor()->GetActorTransform();

		TArray<FVector> snapPoints; // 조인트 위치값
		TArray<FRotator> snapDirection; // 조인트 방향

		FVector closestPoint = FVector::ZeroVector;
		float distance = 0;
		int32 ClosestSnapPointIndex = 0;

		auto* heldActor = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner()); // 잡은 엑터
		if (Cast<APSH_BlockActor>(hitActor)) // 범위 안에 있는 상태
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
			if (snapPoints.IsEmpty()) // 비었다면
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
			if (snapPoints.IsEmpty()) // 비었다면
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

	}
	else
	{
		FVector newLoc = FMath::VInterpTo(rotationHelper->GetComponentLocation(), endLoc, GetWorld()->DeltaTimeSeconds, 5.0f); // 속도를 높여 빠르게 이동
		rotationHelper->SetWorldLocation(newLoc);

		// 부드러운 보간으로 목표 회전으로 이동
		FRotator newRot = FMath::RInterpTo(rotationHelper->GetComponentRotation(), rotationOffset, GetWorld()->DeltaTimeSeconds, 5.0f);
		rotationHelper->SetWorldRotation(newRot);	
	}

	MRPC_HandleBlock(rotationHelper->GetComponentLocation(), rotationHelper->GetComponentRotation());


}

FRotator APSH_Player::WorldHelperRotationOffset() // 추가보정
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
					if (heldBlock->mapBlock && actor->mapBlock) // 맵 오브젝트 와 맵 오브젝트
					{
						PRINTLOG(TEXT("heldBlock->mapBlock && actor->mapBlock"));
						heldBlock->Place(actor, worldTransfrom);
						DropBlcok();
						snapPointIndex = 0;
					}
					else if(heldBlock->mapBlock || actor->mapBlock) // 맵 오브젝트가 아님 과 맵 오브젝트
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

	garbagebot->InitializeMovePoint();
}

void APSH_Player::SRPC_GarbageBotSetState_Implementation(EState state)
{
	if (garbagebot == nullptr) return;

	garbagebot->SetState(state);
}

void APSH_Player::BotMoveAndModeChange()
{
	if (bArtKey) // art 눌림
	{
		FHitResult hitresult;
		bool hit = pc->GetHitResultUnderCursor(ECC_Camera, false, hitresult);

		if (hit)
		{
			SRPC_GabageBotMovePoint(hitresult.ImpactPoint);
		}
	}
	else // art 안눌림
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

			APSH_BlockActor* block = Cast<APSH_BlockActor>(hitresult.GetActor());

			if (block)
			{
				switch (block->blockDataType)
				{
				case  EBlockDataType::MOVEANDFLY:

					if (pc->MyChoiceActionWidget)
					{
						pc->MyChoiceActionWidget->SetVisibility(ESlateVisibility::Visible);
						PRINTLOG(TEXT("->MyChoiceActionWidget"));
					}

					PRINTLOG(TEXT("MOVEANDFLY"));
					break;

				case  EBlockDataType::ROTATE:
					PRINTLOG(TEXT("ROTATE"));

					if (pc->RotationWidgetFac)
					{
						pc->RotationWidget = CreateWidget<URotationWidget>(GetWorld(), pc->RotationWidgetFac);
						if (pc->RotationWidget)
						{
							pc->RotationWidget->AddToViewport();
						}
					}
					break;
				}
			}
			else
			{
				if (pc->MyChoiceActionWidget)
				{
					pc->MyChoiceActionWidget->SetVisibility(ESlateVisibility::Hidden);
				}
			}	
		}
		else
		{
			// 선택된 오브젝트가 없거나 특정 컴포넌트가 없는경우 UI 닫기
			if (pc->MyChoiceActionWidget)
			{
				pc->MyChoiceActionWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		// 오브젝트 선택했을 때 컴포넌트가 있으면 UI열리기
		AActor* SelectedActor = hitresult.GetActor();
		
	}
}

FPSH_ObjectData APSH_Player::ParseJsonToBlockData(const FString& JsonString)
{
	FPSH_ObjectData ObjectData;

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		// BlockData 파싱
		const TSharedPtr<FJsonObject>* BlockDataObject;
		if (JsonObject->TryGetObjectField(TEXT("BlockData"), BlockDataObject))
		{
			ObjectData.blockData = ParseJsonObjectToBlockData(*BlockDataObject);
		}

		// bisSave 파싱
		ObjectData.bisSave = JsonObject->GetBoolField(TEXT("bisSave"));
	}

	return ObjectData;
}

FPSH_BlockData APSH_Player::ParseJsonObjectToBlockData(const TSharedPtr<FJsonObject>& JsonObject)
{
	FPSH_BlockData BlockData;

	// ActorClass 파싱
	FString ActorClassName;
	if (JsonObject->TryGetStringField(TEXT("ActorClass"), ActorClassName))
	{
		FString Path = FString::Printf(TEXT("%s"), *ActorClassName);
		UE_LOG(LogTemp, Warning, TEXT("Attempting to load class at path: %s"), *Path);
		BlockData.actor = LoadClass<AActor>(nullptr, *Path);
		if (!BlockData.actor)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load class at path: %s"), *Path);
		}
	}

	// Transform 파싱
	FTransform ParsedTransform;
	FString TransformString;

	if (JsonObject->TryGetStringField(TEXT("Transform"), TransformString))
	{
		// 문자열에서 FTransform으로 변환
		if (ParsedTransform.InitFromString(TransformString))
		{
			// 성공적으로 변환됨
			BlockData.actorTransform = ParsedTransform;
			PRINTLOG(TEXT("ParsedTransform : %s"),*ParsedTransform.ToString());
			PRINTLOG(TEXT("actorTransform : %s"),*BlockData.actorTransform.ToString());
		}
		else
		{
			// 변환 실패
			UE_LOG(LogTemp, Warning, TEXT("Transform 파싱 실패"));
		}
	}

	// FunctionData 파싱
	const TArray<TSharedPtr<FJsonValue>>* FunctionDataArray;
	if (JsonObject->TryGetArrayField(TEXT("FunctionData"), FunctionDataArray))
	{
		for (const TSharedPtr<FJsonValue>& FunctionDataValue : *FunctionDataArray)
		{
			FPSH_FunctionBlockData FunctionData;

			const TSharedPtr<FJsonObject>* FunctionDataObject;
			if (FunctionDataValue->TryGetObject(FunctionDataObject))
			{
				// 숫자 배열 파싱
				const TArray<TSharedPtr<FJsonValue>>* IntArray;
				if (FunctionDataObject->Get()->TryGetArrayField(TEXT("IntArray"), IntArray))
				{
					for (const TSharedPtr<FJsonValue>& Value : *IntArray)
					{
						if (Value->Type == EJson::Number)
						{
							FunctionData.intArray.Add(Value->AsNumber());
						}
					}
				}

				const TArray<TSharedPtr<FJsonValue>>* FloatArray;
				if (FunctionDataObject->Get()->TryGetArrayField(TEXT("FloatArray"), FloatArray))
				{
					for (const TSharedPtr<FJsonValue>& Value : *FloatArray)
					{
						if (Value->Type == EJson::Number)
						{
							FunctionData.floatArray.Add(Value->AsNumber());
						}
					}
				}

				// Boolean 배열 처리
				const TArray<TSharedPtr<FJsonValue>>* BoolArray;
				if (FunctionDataObject->Get()->TryGetArrayField(TEXT("BoolArray"), BoolArray))
				{
					for (const TSharedPtr<FJsonValue>& Value : *BoolArray)
					{
						if (Value->Type == EJson::Boolean)
						{
							FunctionData.boolArray.Add(Value->AsBool());
						}
					}
				}

				// VectorArray 및 RotatorArray는 문자열 배열로 처리
				TArray<FString> VectorStrings;
				if ((*FunctionDataObject)->TryGetStringArrayField(TEXT("VectorArray"), VectorStrings))
				{
					for (const FString& VectorString : VectorStrings)
					{
						FVector ParsedVector;
						if (ParsedVector.InitFromString(VectorString))
						{
							FunctionData.fvectorArray.Add(ParsedVector);
						}
					}
				}

				TArray<FString> RotatorStrings;
				if ((*FunctionDataObject)->TryGetStringArrayField(TEXT("RotatorArray"), RotatorStrings))
				{
					for (const FString& RotatorString : RotatorStrings)
					{
						FRotator ParsedRotator;
						if (ParsedRotator.InitFromString(RotatorString))
						{
							FunctionData.frotatorArray.Add(ParsedRotator);
						}
					}
				}

				BlockData.funcitonData.Add(FunctionData);
			}
		}
	}

	// Children 파싱
	const TArray<TSharedPtr<FJsonValue>>* ChildrenArray;
	if (JsonObject->TryGetArrayField(TEXT("Children"), ChildrenArray))
	{
		for (const TSharedPtr<FJsonValue>& ChildValue : *ChildrenArray)
		{
			const TSharedPtr<FJsonObject>* ChildObject;
			if (ChildValue->TryGetObject(ChildObject))
			{
				BlockData.childData.Add(ParseJsonObjectToBlockData(*ChildObject));
			}
		}
	}

	return BlockData;
}

void APSH_Player::SpawnBlocksFromJson(const FString& JsonString)
{
	// JSON 데이터를 FPSH_ObjectData로 변환
	FPSH_ObjectData ObjectData = ParseJsonToBlockData(JsonString);
	APSH_BlockActor* RootBlock = SpawnBlock(ObjectData.blockData, nullptr);

// 	for (FPSH_BlockData data : ObjectData.blockData.childData)
// 	{
// 		APSH_BlockActor* ChildeBlock = SpawnBlock(data, nullptr);
// 
// 	}
	if (RootBlock)
	{
		
		PRINTLOG(TEXT("Root block spawned successfully."));
	}
	else
	{
		PRINTLOG(TEXT("Failed to spawn root block."));
	}
}

FPSH_WorldData APSH_Player::ParseJsonToWorldData(const FString& JsonString)
{
	FPSH_WorldData WorldData;

	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	TSharedPtr<FJsonObject> JsonObject;

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		PRINTLOG(TEXT("Failed to parse JSON string into WorldData."));
		return WorldData;
	}

	if (!JsonObject->HasField(TEXT("BlockArray")))
	{
		PRINTLOG(TEXT("JSON does not contain BlockArray field."));
		return WorldData;
	}

	TArray<TSharedPtr<FJsonValue>> BlockArray = JsonObject->GetArrayField(TEXT("BlockArray"));
	PRINTLOG(TEXT("Parsed BlockArray with %d elements."), BlockArray.Num());

	for (const TSharedPtr<FJsonValue>& BlockValue : BlockArray)
	{
		TSharedPtr<FJsonObject> BlockObject = BlockValue->AsObject();
		if (BlockObject.IsValid())
		{
			FPSH_BlockData BlockData = ConvertJsonObjectToBlockData(BlockObject);
			FPSH_ObjectData ObjectData;
			ObjectData.blockData = BlockData;

			WorldData.BlockArray.Add(ObjectData);

			PRINTLOG(TEXT("Successfully parsed block data: %s"), *BlockData.actor->GetName());
		}
		else
		{
			PRINTLOG(TEXT("Failed to parse block object in JSON."));
		}
	}

	return WorldData;
}
void APSH_Player::SpawnBlocksFromObjectData(const FPSH_ObjectData& ObjectData)
{
	// 최상위 블록 스폰
	APSH_BlockActor* RootBlock = SpawnBlock(ObjectData.blockData, nullptr);
	if (RootBlock)
	{
		PRINTLOG(TEXT("Root block spawned successfully."));
	}
	else
	{
		PRINTLOG(TEXT("Failed to spawn root block."));
	}
}
FPSH_BlockData APSH_Player::ConvertJsonObjectToBlockData(const TSharedPtr<FJsonObject>& JsonObject)
{
	FPSH_BlockData BlockData;

	FString ActorClassPath = JsonObject->GetStringField(TEXT("ActorClass"));
	UClass* ActorClass = LoadClass<AActor>(nullptr, *ActorClassPath);
	if (!ActorClass)
	{
		PRINTLOG(TEXT("Failed to load ActorClass from path: %s"), *ActorClassPath);
	}
	BlockData.actor = ActorClass;

	FString TransformString = JsonObject->GetStringField(TEXT("Transform"));
	FTransform Transform;
	if (!Transform.InitFromString(TransformString))
	{
		PRINTLOG(TEXT("Failed to parse Transform from string: %s"), *TransformString);
	}
	BlockData.actorTransform = Transform;

	// FunctionData 복원
	TArray<TSharedPtr<FJsonValue>> FunctionDataArray = JsonObject->GetArrayField(TEXT("FunctionData"));
	for (const TSharedPtr<FJsonValue>& FunctionValue : FunctionDataArray)
	{
		TSharedPtr<FJsonObject> FunctionObject = FunctionValue->AsObject();
		FPSH_FunctionBlockData FunctionData;
		FunctionData.intArray = ConvertJsonToIntArray(FunctionObject->GetArrayField(TEXT("IntArray")));
		FunctionData.floatArray = ConvertJsonToFloatArray(FunctionObject->GetArrayField(TEXT("FloatArray")));
		FunctionData.fvectorArray = ConvertJsonToVectorArray(FunctionObject->GetArrayField(TEXT("VectorArray")));
		FunctionData.frotatorArray = ConvertJsonToRotatorArray(FunctionObject->GetArrayField(TEXT("RotatorArray")));
		FunctionData.boolArray = ConvertJsonToBoolArray(FunctionObject->GetArrayField(TEXT("BoolArray")));
		BlockData.funcitonData.Add(FunctionData);
	}

	// Children 복원
	TArray<TSharedPtr<FJsonValue>> ChildrenArray = JsonObject->GetArrayField(TEXT("Children"));
	for (const TSharedPtr<FJsonValue>& ChildValue : ChildrenArray)
	{
		TSharedPtr<FJsonObject> ChildObject = ChildValue->AsObject();
		if (ChildObject.IsValid())
		{
			FPSH_BlockData ChildData = ConvertJsonObjectToBlockData(ChildObject);
			BlockData.childData.Add(ChildData);
		}
	}

	return BlockData;
}
APSH_BlockActor* APSH_Player::SpawnBlock(const FPSH_BlockData& BlockData, APSH_BlockActor* Parent)
{
// 기존
// 	if (!BlockData.actor)
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("Invalid actor class for block data."));
// 		return nullptr;
// 	}
// 
// 	FActorSpawnParameters SpawnParams;
// 	APSH_BlockActor* NewBlock = GetWorld()->SpawnActor<APSH_BlockActor>(
// 		BlockData.actor,
// 		BlockData.actorTransform,
// 		SpawnParams
// 	);
// 
// 	if (NewBlock)
// 	{
// 		NewBlock->SetOwner(this);
// 
// 		// 물리 비활성화 및 태그 제거를 모든 클라이언트에서 동기화
// 		NewBlock->MRPC_LoadSetting();
// 
// 		// 부모와 연결
// 		if (Parent)
// 		{
// 			NewBlock->AttachToActor(Parent, FAttachmentTransformRules::KeepWorldTransform);
// 
// 			// 부모-자식 관계 설정
// 			NewBlock->SetActorRelativeLocation(BlockData.actorTransform.GetLocation());
// 			NewBlock->SetActorRotation(BlockData.actorTransform.GetRotation());
// 			NewBlock->SetActorRelativeScale3D(BlockData.actorTransform.GetScale3D());
// 		}
// 
// 		// 컴포넌트 데이터 로드
// 		if (!BlockData.funcitonData.IsEmpty())
// 		{
// 			NewBlock->ComponentLoadData(BlockData.funcitonData);
// 		}
// 
// 		// 자식 블록 생성
// 		for (const FPSH_BlockData& ChildData : BlockData.childData)
// 		{
// 			SpawnBlock(ChildData, NewBlock);
// 		}
// 	}
// 	else
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("Failed to spawn actor of class: %s"), *BlockData.actor->GetName());
// 	}
// 
// 	return NewBlock;

	if (!BlockData.actor)
	{
		PRINTLOG(TEXT("Actor class is invalid for block data. Cannot spawn block."));
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	APSH_BlockActor* NewBlock = GetWorld()->SpawnActor<APSH_BlockActor>(
		BlockData.actor,
		BlockData.actorTransform,
		SpawnParams
	);

	if (NewBlock)
	{
		NewBlock->SetOwner(this);
		NewBlock->MRPC_LoadSetting();
		// 부모와 연결
		if (Parent)
		{
			// 물리 비활성화 및 태그 제거를 모든 클라이언트에서 동기화
			
			NewBlock->AttachToActor(Parent, FAttachmentTransformRules::KeepWorldTransform);

			// 부모-자식 관계 설정
			NewBlock->SetActorRelativeLocation(BlockData.actorTransform.GetLocation());
			NewBlock->SetActorRotation(BlockData.actorTransform.GetRotation());
			NewBlock->SetActorRelativeScale3D(BlockData.actorTransform.GetScale3D());
		}

		// 컴포넌트 데이터 로드
		if (!BlockData.funcitonData.IsEmpty())
		{
			NewBlock->ComponentLoadData(BlockData.funcitonData);
		}

		// 자식 블록 생성
		for (const FPSH_BlockData& ChildData : BlockData.childData)
		{
			APSH_BlockActor* ChildBlock = SpawnBlock(ChildData, NewBlock);
			if (ChildBlock)
			{
				PRINTLOG(TEXT("Child block spawned and attached successfully."));
			}
		}
	}
	else
	{
		PRINTLOG(TEXT("Failed to spawn actor of class: %s"), *BlockData.actor->GetName());
	}

	return NewBlock;
}


void APSH_Player::LoadTest()
{
	SRPC_Load(jsonString);
}

void APSH_Player::SRPC_Load_Implementation(const FString& JsonString)
{
	// JSON 문자열을 FPSH_WorldData로 변환
	FPSH_WorldData WorldData = ParseJsonToWorldData(JsonString);

	// 파싱한 데이터를 사용하여 블록 생성
	for (const FPSH_ObjectData& ObjectData : WorldData.BlockArray)
	{
		SpawnBlocksFromObjectData(ObjectData);
	};
}
// 	if (!IsValid(dataTable))
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("Invalid DataTable reference in SRPC_Load"));
// 		return;
// 	}
// 
// 	TArray<FPSH_ObjectData*> dataArray;
// 	dataTable->GetAllRows<FPSH_ObjectData>(TEXT("LoadContext"), dataArray);
// 
// 	for (FPSH_ObjectData* data : dataArray)
// 	{
// 		if (!data) continue;
// 
// 		TSubclassOf<APSH_BlockActor> spawnActor = data->blockData.actor;
// 
// 		if (spawnActor)
// 		{
// 			FActorSpawnParameters Params;
// 			APSH_BlockActor* rootBlock = GetWorld()->SpawnActor<APSH_BlockActor>(
// 				spawnActor,
// 				data->blockData.actorTransform,
// 				Params
// 			);
// 
// 			if (rootBlock)
// 			{
// 				rootBlock->SetOwner(this);
// 				if (rootBlock->blockDataType == EBlockDataType::MOVEANDFLY)
// 				{
// 					rootBlock->SetActorScale3D(FVector(0.5f, 0.48f, 0.5f));
// 				}
// 
// 				if (!data->blockData.funcitonData.IsEmpty())
// 				{
// 					rootBlock->ComponentLoadData(data->blockData.funcitonData);
// 				}
// 				// 자식 블록 로드
// 				rootBlock->LoadBlockHierarchy(data->blockData);
// 			}
// 		}
// 	}


void APSH_Player::SRPC_SpawnBlock_Implementation(TSubclassOf<class APSH_BlockActor> spawnActor)
{
	if (bSpawn == false) return;
	// 캐릭터의 위치와 방향을 기준으로 상대적 위치를 설정
	FVector CharacterLocation = GetActorLocation();
	FVector upVector = GetActorUpVector();  // 캐릭터의 오른쪽 방향
	FVector ForwardVector = GetActorForwardVector();  // 캐릭터의 앞쪽 방향

	// 상대적 오프셋 설정 (오른쪽으로 200, 앞쪽으로 약간 떨어진 위치)
	FVector SpawnLocation = CharacterLocation + (upVector * 300) + (ForwardVector * 300);

	// 스폰 파라미터 설정 및 엑터 소환
	FActorSpawnParameters SpawnParams;

	APSH_BlockActor* SpawnActor = GetWorld()->SpawnActor<APSH_BlockActor>(spawnActor, SpawnLocation, GetActorRotation(), SpawnParams);

	SpawnActor->SetOwner(this);
}

FString APSH_Player::ConvertBlockDataToJson(const FPSH_ObjectData& BlockData)
{
	TSharedPtr<FJsonObject> RootJsonObject = MakeShared<FJsonObject>();

	// blockData를 JSON으로 변환
	TSharedPtr<FJsonObject> BlockDataJsonObject = ConvertBlockDataToJsonObject(BlockData.blockData);

	// 최상위 JSON 객체에 추가
	RootJsonObject->SetObjectField(TEXT("BlockData"), BlockDataJsonObject);
	RootJsonObject->SetBoolField(TEXT("bisSave"), BlockData.bisSave);

	// JSON 문자열로 직렬화
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	if (FJsonSerializer::Serialize(RootJsonObject.ToSharedRef(), Writer))
	{
		return OutputString;
	}

	return TEXT(""); // 실패 시 빈 문자열 반환
}

FString APSH_Player::ConvertWorldDataToJson(const FPSH_WorldData& WorldData)
{
	TSharedRef<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	TArray<TSharedPtr<FJsonValue>> BlockArray;
	for (const FPSH_ObjectData& BlockData : WorldData.BlockArray)
	{
		TSharedPtr<FJsonObject> BlockObject = ConvertBlockDataToJsonObject(BlockData.blockData);

		if (BlockObject.IsValid())
		{
			BlockArray.Add(MakeShareable(new FJsonValueObject(BlockObject)));
		}
		else
		{
			PRINTLOG(TEXT("Failed to convert block data to JSON object."));
		}
	}

	JsonObject->SetArrayField(TEXT("BlockArray"), BlockArray);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	if (FJsonSerializer::Serialize(JsonObject, Writer))
	{
		PRINTLOG(TEXT("Serialized WorldData JSON: %s"), *OutputString);
	}
	else
	{
		PRINTLOG(TEXT("Failed to serialize WorldData JSON."));
	}

	return OutputString;
}
TSharedPtr<FJsonObject> APSH_Player::ConvertBlockDataToJsonObject(const FPSH_BlockData& BlockData)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

	// 기본 데이터 필드 추가
	JsonObject->SetStringField(TEXT("ActorClass"), BlockData.actor->GetPathName());
	JsonObject->SetStringField(TEXT("Transform"), BlockData.actorTransform.ToString());

	// funcitonData 배열 변환
	TArray<TSharedPtr<FJsonValue>> FunctionDataArray;
	for (const FPSH_FunctionBlockData& FunctionData : BlockData.funcitonData)
	{
		TSharedPtr<FJsonObject> FunctionDataObject = MakeShared<FJsonObject>();
		FunctionDataObject->SetArrayField(TEXT("IntArray"), ConvertIntArrayToJson(FunctionData.intArray));
		FunctionDataObject->SetArrayField(TEXT("FloatArray"), ConvertFloatArrayToJson(FunctionData.floatArray));
		FunctionDataObject->SetArrayField(TEXT("VectorArray"), ConvertVectorArrayToJson(FunctionData.fvectorArray));
		FunctionDataObject->SetArrayField(TEXT("RotatorArray"), ConvertRotatorArrayToJson(FunctionData.frotatorArray));
		FunctionDataObject->SetArrayField(TEXT("BoolArray"), ConvertBoolArrayToJson(FunctionData.boolArray));
		FunctionDataArray.Add(MakeShared<FJsonValueObject>(FunctionDataObject));
	}
	JsonObject->SetArrayField(TEXT("FunctionData"), FunctionDataArray);

	// childData 배열 변환
	TArray<TSharedPtr<FJsonValue>> ChildArray;
	for (const FPSH_BlockData& ChildData : BlockData.childData)
	{
		TSharedPtr<FJsonObject> ChildJsonObject = ConvertBlockDataToJsonObject(ChildData);
		if (ChildJsonObject.IsValid())
		{
			ChildArray.Add(MakeShareable(new FJsonValueObject(ChildJsonObject)));
		}
	}
	JsonObject->SetArrayField(TEXT("Children"), ChildArray);

	return JsonObject;
}
TArray<TSharedPtr<FJsonValue>> APSH_Player::ConvertIntArrayToJson(const TArray<int32>& IntArray)
{
	TArray<TSharedPtr<FJsonValue>> JsonArray;
	for (int32 Value : IntArray)
	{
		JsonArray.Add(MakeShared<FJsonValueNumber>(Value));
	}
	return JsonArray;
}
TArray<TSharedPtr<FJsonValue>> APSH_Player::ConvertFloatArrayToJson(const TArray<float>& FloatArray)
{
	TArray<TSharedPtr<FJsonValue>> JsonArray;
	for (float Value : FloatArray)
	{
		JsonArray.Add(MakeShared<FJsonValueNumber>(Value));
	}
	return JsonArray;
}
TArray<TSharedPtr<FJsonValue>> APSH_Player::ConvertVectorArrayToJson(const TArray<FVector>& VectorArray)
{
	TArray<TSharedPtr<FJsonValue>> JsonArray;
	for (const FVector& Vector : VectorArray)
	{
		JsonArray.Add(MakeShared<FJsonValueString>(Vector.ToString()));
	}
	return JsonArray;
}
TArray<TSharedPtr<FJsonValue>> APSH_Player::ConvertRotatorArrayToJson(const TArray<FRotator>& RotatorArray)
{
	TArray<TSharedPtr<FJsonValue>> JsonArray;
	for (const FRotator& Rotator : RotatorArray)
	{
		JsonArray.Add(MakeShared<FJsonValueString>(Rotator.ToString()));
	}
	return JsonArray;
}
TArray<TSharedPtr<FJsonValue>> APSH_Player::ConvertBoolArrayToJson(const TArray<bool>& BoolArray)
{
	TArray<TSharedPtr<FJsonValue>> JsonArray;
	for (bool Value : BoolArray)
	{
		JsonArray.Add(MakeShared<FJsonValueBoolean>(Value));
	}
	return JsonArray;
}

TArray < int32 > APSH_Player::ConvertJsonToIntArray(const TArray<TSharedPtr<FJsonValue>>& JsonArray)
{
	TArray<int32> IntArray;
	for (const TSharedPtr<FJsonValue>& Value : JsonArray)
	{
		IntArray.Add(Value->AsNumber());
	}
	return IntArray;
}
TArray<float> APSH_Player::ConvertJsonToFloatArray(const TArray<TSharedPtr<FJsonValue>>& JsonArray)
{
	TArray<float> FloatArray;
	for (const TSharedPtr<FJsonValue>& Value : JsonArray)
	{
		FloatArray.Add(Value->AsNumber());
	}
	return FloatArray;
}
TArray<FVector> APSH_Player::ConvertJsonToVectorArray(const TArray<TSharedPtr<FJsonValue>>& JsonArray)
{
	TArray<FVector> VectorArray;
	for (const TSharedPtr<FJsonValue>& Value : JsonArray)
	{
		FVector Vector;
		Vector.InitFromString(Value->AsString());
		VectorArray.Add(Vector);
	}

	return VectorArray;
}
TArray<FRotator> APSH_Player::ConvertJsonToRotatorArray(const TArray<TSharedPtr<FJsonValue>>& JsonArray)
{
	TArray<FRotator> RotatorArray;
	for (const TSharedPtr<FJsonValue>& Value : JsonArray)
	{
		FRotator Rotator;
		Rotator.InitFromString(Value->AsString());
		RotatorArray.Add(Rotator);
	}
	return RotatorArray;
}
TArray<bool> APSH_Player::ConvertJsonToBoolArray(const TArray<TSharedPtr<FJsonValue>>& JsonArray)
{
	TArray<bool> BoolArray;
	for (const TSharedPtr<FJsonValue>& Value : JsonArray)
	{
		BoolArray.Add(Value->AsBool());
	}
	return BoolArray;
}

void APSH_Player::SaveTest()
{
	/*SRPC_Save();*/

	if (!IsValid(dataTable))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid DataTable reference in SRPC_Save"));
		return;
	}

	TArray<AActor*> blockArray;
	// "owner" 태그가 달린 모든 블록을 가져옴
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APSH_BlockActor::StaticClass(), FName(TEXT("owner")), blockArray);

	FPSH_WorldData WorldData;

	for (AActor* arrayActor : blockArray)
	{
		APSH_BlockActor* blockActor = Cast<APSH_BlockActor>(arrayActor);

		if (blockActor)
		{
			// 계층 구조 저장
			FPSH_ObjectData BlockData = blockActor->SaveBlock();
			WorldData.BlockArray.Add(BlockData);			
		}
	}

	jsonString = ConvertWorldDataToJson(WorldData);
// 	if (!jsonString.IsEmpty())
// 	{
// 		PRINTLOG(TEXT("WorldData JSON: %s"), *jsonString);
// 
// 		// 파일로 저장
// 		if (SaveJsonToFile(jsonString, "WorldData.json"))
// 		{
// 			PRINTLOG(TEXT("JSON saved successfully to file!"));
// 		}
// 		else
// 		{
// 			PRINTLOG(TEXT("Failed to save JSON to file."));
// 		}
// 	}
}

void APSH_Player::ShowInterface()
{
	if(!bCreatingMode) return;
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
	//크기 조정 +=
}
