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

// Sets default values
APSH_Player::APSH_Player()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 스프링 컴포넌트
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	springArm->SetupAttachment(RootComponent);

	// 카메라 컴포넌트
	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	cameraComp->SetupAttachment(springArm);

	// 피직스 핸들 컴포넌트
	handleComp = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("Handle"));
}

// Called when the game starts or when spawned
void APSH_Player::BeginPlay()
{
	Super::BeginPlay();
	
	// 마우스 위젯 사용 
	mouseWidget = Cast<UPSH_MouseWidget>(CreateWidget(GetWorld(), mouseWidgetFac));

	if (mouseWidget)
	{
		pc->SetMouseCursorWidget(EMouseCursor::Default, mouseWidget);
		pc->SetInputMode(FInputModeGameAndUI());
		pc->SetShowMouseCursor(true);
		pc->bEnableClickEvents = true;
		pc->bEnableMouseOverEvents = true;
	}

}

// Called every frame
void APSH_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APSH_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	pc = Cast<APlayerController>(GetController());

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

	if (handleComp->GetGrabbedComponent() != nullptr) // 잡은 게 있다면
	{
		FLocationPoint Point = PreTraceChek();
		ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);
		FHitResult OutHit;

		UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			Point.startLocation,
			Point.endLocation,
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

		UE_LOG(LogTemp, Warning, TEXT("GetGrabbedComponent->Owner : %s"), *handleComp->GetGrabbedComponent()->GetOwner()->GetName());
	}
	else // 잡은게 없다면
	{
		FHitResult outHit;

		if (pc && pc->GetHitResultUnderCursor(ECC_Visibility, false, outHit))
		{
			APSH_BlockActor* blockActor = Cast<APSH_BlockActor>(outHit.GetActor());
			if (blockActor)
			{
				UE_LOG(LogTemp, Warning, TEXT("blockActorName : %s"), *blockActor->GetName());
				blockActor->PickUp(handleComp);
				blockActor->SetOwner(pc);
				blockActor->bGrab = true;
			}
		}
	}
}
FLocationPoint APSH_Player::PreTraceChek()
{
	FLocationPoint Loc;
	auto* ChildBlcak = Cast<APSH_BlockActor>(handleComp->GetGrabbedComponent()->GetOwner()); // Array에 저장

	FVector WorldLoc;
	FVector WorldDir;

	pc->DeprojectMousePositionToWorld(WorldLoc, WorldDir);

	if (handleComp->GetGrabbedComponent())
	{
		actorsToIgnore = ChildBlcak->childsActor;
		actorsToIgnore.Add(handleComp->GetGrabbedComponent()->GetOwner());
	}

	FVector CameraLoc = cameraComp->GetComponentLocation();
	Loc.startLocation = CameraLoc;
	Loc.endLocation = CameraLoc + playerReach * WorldDir;



	return Loc;
}
FHitResult APSH_Player::CastRay()
{
	FHitResult hitInfo;

	FVector worldLoc;
	FVector worldDir;
	pc->DeprojectMousePositionToWorld(worldLoc, worldDir);

	// 잡는거구나
	FVector StartLoc = cameraComp->GetComponentLocation();
	FVector EndLoc = StartLoc + (rayPower * worldDir);

	FCollisionQueryParams prams;
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, StartLoc, EndLoc, ECC_Visibility, prams);

	if (bHit)
	{
		DrawDebugSphere(GetWorld(), hitInfo.ImpactPoint, 10, 10, FColor::Red, false, 5);
	}

	return hitInfo;
}