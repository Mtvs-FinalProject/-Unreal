// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/PSH_Actor/PSH_BlockActor.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

// Sets default values
APSH_BlockActor::APSH_BlockActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(meshComp);
}

// Called when the game starts or when spawned
void APSH_BlockActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APSH_BlockActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bGrab) // true �ϋ�
	{
		FVector worldLoc; // ���콺�� ���� ������
		FVector worldDir; // ī�޶� �����ǰ� ���콺 Ŭ�� ��Ұ��� ����

		APlayerController* pc = Cast<APlayerController>(GetOwner());
		UE_LOG(LogTemp, Warning, TEXT("%s"), *GetOwner()->GetName());

		if (pc == nullptr) return;
		pc->DeprojectMousePositionToWorld(worldLoc, worldDir);

		/*UE_LOG(LogTemp, Warning, TEXT("X : %f Y : %f Z : %f"), worldLoc.X, worldLoc.Y, worldLoc.Z);*/
		FVector TargetLocation = worldLoc + (worldDir * 200.0f); // �ణ �������� �̵�

		playerHandle->SetTargetLocation(TargetLocation);
	}
}
void APSH_BlockActor::PickUp(class UPhysicsHandleComponent* handle)
{
	if (handle == nullptr) return;

	playerHandle = handle;
	playerHandle->GrabComponentAtLocationWithRotation(meshComp, NAME_None, GetActorLocation(), GetActorRotation());
}
