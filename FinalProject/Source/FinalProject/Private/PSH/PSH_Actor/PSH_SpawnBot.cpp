// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/PSH_Actor/PSH_SpawnBot.h"
#include "PSH/PSH_Player/PSH_Player.h"
#include "Components/SpotLightComponent.h"
#include "PSH/PSH_Player/PSH_PlayerController.h"
#include "PSH/PSH_UI/PSH_ObjectWidget.h"
#include "PSH/PSH_Actor/PSH_BlockActor.h"
#include "../FinalProject.h"

// Sets default values
APSH_SpawnBot::APSH_SpawnBot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	compMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	compMesh->SetupAttachment(RootComponent);
	compMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tmepMesh(TEXT("/Script/Engine.StaticMesh'/Game/YWK/UFO/SpawnBot.SpawnBot'"));

	if (tmepMesh.Succeeded())
	{
		compMesh->SetStaticMesh(tmepMesh.Object);
	}

	ConstructorHelpers::FObjectFinder<UMaterial> downMeshMat(TEXT("/Script/Engine.Material'/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial'"));
	ConstructorHelpers::FObjectFinder<UMaterial> blueLightMat(TEXT("/Script/Engine.Material'/Game/YWK/Effect/M_BlueLight.M_BlueLight'"));
	ConstructorHelpers::FObjectFinder<UMaterial> redLightMAt(TEXT("/Script/Engine.Material'/Game/YWK/Effect/M_RedLight.M_RedLight'"));

	if (downMeshMat.Succeeded() && blueLightMat.Succeeded() && redLightMAt.Succeeded())
	{
		matArray.Add(downMeshMat.Object);
		matArray.Add(blueLightMat.Object);
		matArray.Add(redLightMAt.Object);
	}

	downMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DownMesh"));
	downMesh->SetupAttachment(compMesh);
	downMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tmepDownMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));

	if (tmepDownMesh.Succeeded())
	{
		downMesh->SetStaticMesh(tmepDownMesh.Object);
		downMesh->SetMaterial(0,matArray[0]);
	}

	lightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightMesh"));
	lightMesh->SetupAttachment(compMesh);
	lightMesh->SetRelativeLocation(FVector(0.f, 0.f, -270.f));
	lightMesh->SetRelativeScale3D(FVector(6.f));
	lightMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	lightMesh->SetVisibility(false);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tmeplightMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cone.Cone'"));
	if (tmeplightMesh.Succeeded())
	{
		lightMesh->SetStaticMesh(tmeplightMesh.Object);
	}

	spotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	spotLight->SetupAttachment(compMesh);
	spotLight->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	spotLight->Intensity = 50878;
	spotLight->IntensityUnits = ELightUnits::Unitless;

	bReplicates = true;
	SetReplicateMovement(true);

}

// Called when the game starts or when spawned
void APSH_SpawnBot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APSH_SpawnBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(player == nullptr) return;

	const FString myState = UEnum::GetValueAsString(state);
	DrawDebugString(GetWorld(), GetActorLocation(), myState, nullptr, FColor::Red, 0, true);

	switch (state)
	{
	case EspawnState::IDLE: IdleState();
		break;
	case EspawnState::SPAWNMOVE: SpawnMoveState(DeltaTime);
		break;
	case EspawnState::IDLEMOVE: IdleMoveState(DeltaTime);
		break;
	case EspawnState::SPAWN: SpawnState(DeltaTime);
		break;
	}
}
void APSH_SpawnBot::SetPlayer(class APSH_Player* playerClass)
{
	player = playerClass;
}

void APSH_SpawnBot::MoveTo()
{
	SetState(EspawnState::SPAWNMOVE);
	SetActorHiddenInGame(false);
}

void APSH_SpawnBot::IdleState()
{
	FVector playerLoc = player->GetActorLocation();
	FVector playerForward = player->GetActorForwardVector(); // 플레이어의 앞 방향
	FVector playerUp = player->GetActorUpVector(); // 플레이어의 위 방향

	// 대각선 방향으로  위치 계산
	FVector targetLocation = playerLoc + (playerForward * 5000) + (playerUp * 7000);
	SetActorHiddenInGame(true);
	SetActorLocation(targetLocation);
}

void APSH_SpawnBot::SpawnMoveState(const float& deltaTime)
{
	FVector playerLoc = player->GetActorLocation();
	FVector myLoc = GetActorLocation();
	FVector upVector = player->GetActorUpVector();  // 캐릭터의 오른쪽 방향
	FVector forwardVector = player->GetActorForwardVector();  // 캐릭터의 위쪽 방향

	TagetLoc = playerLoc + (upVector * 400) + (forwardVector * 300);

	// 현재 위치와 타겟 위치가 가까우면 이동 멈춤
	if (FVector::Dist(myLoc, TagetLoc) <= 10.f) // 10.f는 허용 오차 범위
	{
		SetActorLocation(TagetLoc); // 정확한 위치에 도달하도록 설정
		APSH_PlayerController* PlayerController = Cast<APSH_PlayerController>(GetOwner()->GetOwner());
		if (PlayerController) // 위젯 보이기
		{
			PlayerController->CRPC_ShowObjectWidget();
		}
		
		MRPC_SetMat(1);
		SetState(EspawnState::SPAWN);
	}
	else
	{
		// VInterpTo를 이용한 부드러운 이동
		FVector NewLocation = FMath::VInterpTo(myLoc, TagetLoc, deltaTime, 10);
		SetActorLocation(NewLocation);
	}
	FRotator TargetRotation = FRotationMatrix::MakeFromX(myLoc - TagetLoc).Rotator();

	// 부드럽게 회전하도록 InterpTo 적용 
	FRotator newRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, deltaTime, 3.0f);
	SetActorRotation(newRotation);
	
}

void APSH_SpawnBot::IdleMoveState(const float& deltaTime)
{
	FVector playerLoc = player->GetActorLocation();
	FVector playerForward = player->GetActorForwardVector(); // 플레이어의 앞 방향
	FVector playerUp = player->GetActorUpVector(); // 플레이어의 위 방향
	FVector myLoc = GetActorLocation();
	idleTime += deltaTime;
	// 대각선 방향으로 5000 유닛 떨어진 위치 계산
	TagetLoc = playerLoc + (playerForward * 3000) + (playerUp * 4000);
	if (idleTime > 2) 
	{
		SetState(EspawnState::IDLE);
		idleTime = 0;
	}
	else
	{
		// VInterpTo를 이용한 부드러운 이동
		SetActorLocation(myLoc + TagetLoc.GetSafeNormal() * 5000 * deltaTime);
	}
	FRotator TargetRotation = FRotationMatrix::MakeFromX(myLoc - TagetLoc).Rotator();

	// 부드럽게 회전하도록 InterpTo 적용 
	FRotator newRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, deltaTime, 3.0f);
	SetActorRotation(newRotation);
}
void APSH_SpawnBot::SpawnState(const float& deltaTime)
{
	FVector playerLoc = player->GetActorLocation();
	FVector myLoc = GetActorLocation();
	FVector upVector = player->GetActorUpVector();  // 캐릭터의 오른쪽 방향
	FVector forwardVector = player->GetActorForwardVector();  // 캐릭터의 위쪽 방향

	TagetLoc = playerLoc + (upVector * 400) + (forwardVector * 300);
	// 플레이어가 물체를 바라보도록 회전 계산
	FRotator TargetRotation = FRotationMatrix::MakeFromX(myLoc - TagetLoc).Rotator();

	// 부드럽게 회전하도록 InterpTo 적용 
	FRotator newRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, deltaTime, 3.0f);
	newRotation.Pitch = 0;

	SetActorLocationAndRotation(TagetLoc,newRotation);

	LineChek();
}

void APSH_SpawnBot::SetState(EspawnState State)
{
	state = State;

	if(state == EspawnState::IDLEMOVE)
	{
		MRPC_SetVisible(false);
	}
}

void APSH_SpawnBot::MRPC_SetVisible_Implementation(bool chek)
{
	if (lightMesh == nullptr) return;
	
	lightMesh->SetVisibility(chek);

	/*if(chek == false) */
}
void APSH_SpawnBot::MRPC_SetMat_Implementation(int32 arrayIndex)
{
	if(lightMesh == nullptr) return;
	
	if(!lightMesh->IsVisible()) MRPC_SetVisible(true);

	lightMesh->SetMaterial(0, matArray[arrayIndex]);

}
void APSH_SpawnBot::LineChek()
{
	FHitResult hitInfo;
	FVector startLoc = GetActorLocation();
	FVector endLoc = startLoc + (GetActorUpVector() * -100) ;
	FQuat quat = FQuat::Identity;
	float radius = 150.f; // 구체의 반경
	ECollisionChannel TraceChannel = ECC_Visibility;
	
	bool bHit = GetWorld()->SweepSingleByChannel(
		hitInfo,
		startLoc,
		endLoc,
		quat,
		TraceChannel,
		FCollisionShape::MakeSphere(radius)
	);

// 	DrawDebugSphere(GetWorld(), startLoc, radius, 12, FColor::Red, false, 1.0f); // 시작 위치
// 	DrawDebugSphere(GetWorld(), endLoc, radius, 12, FColor::Blue, false, 1.0f); // 끝 위치

	if (bHit)
	{
		/*DrawDebugSphere(GetWorld(), hitInfo.Location, radius, 12, FColor::Yellow, false, 1.0f);*/
		if (Cast<APSH_BlockActor>(hitInfo.GetActor()))
		{
			
			MRPC_SetMat(2);
			if(player == nullptr) return;

			player->bSpawn = false;
		}
	}
	else
	{
		MRPC_SetMat(1);

		if (player == nullptr) return;

		player->bSpawn = true;
	}
}