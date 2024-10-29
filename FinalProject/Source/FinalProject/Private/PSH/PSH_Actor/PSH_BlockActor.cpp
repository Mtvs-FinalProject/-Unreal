// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/PSH_Actor/PSH_BlockActor.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetArrayLibrary.h"
#include "PSH/PSH_Player/PSH_Player.h"
#include "Net/UnrealNetwork.h"
#include "YWK/MyMoveActorComponent.h"
#include "YWK/MyFlyActorComponent.h"
#include "YWK/MyRotateActorComponent.h"

// Sets default values
APSH_BlockActor::APSH_BlockActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(meshComp);

	
	bReplicates = true;
	SetReplicateMovement(true);

	// ��� ������Ʈ��
	MyMoveActorComponent = CreateDefaultSubobject<UMyMoveActorComponent>(TEXT("MoveComponent"));
	MyFlyActorComponent = CreateDefaultSubobject<UMyFlyActorComponent>(TEXT("FlyComponent"));
	MyRotateActorComponent = CreateDefaultSubobject<UMyRotateActorComponent>(TEXT("RotateComponent"));

	// ������Ʈ�� Ư�� ��Ȳ������ Ȱ��ȭ�ǵ��� ����
	MyMoveActorComponent->SetActive(false);
	MyFlyActorComponent->SetActive(false);
	MyRotateActorComponent->SetActive(false);
}

// Called when the game starts or when spawned
void APSH_BlockActor::BeginPlay()
{
	Super::BeginPlay();
	
	APSH_Player * player = Cast<APSH_Player>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	SetOwner(player);

	meshComp->OnComponentSleep.AddDynamic(this, &APSH_BlockActor::OnComponentSleep);

	if (MyMoveActorComponent && MyMoveActorComponent->IsComponentTickEnabled())
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveComponent is active in %s"), *GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveComponent is NOT active in %s"), *GetName());
		MyMoveActorComponent->SetComponentTickEnabled(true);  // ������Ʈ Ȱ��ȭ ����
	}

	if (MyFlyActorComponent && MyFlyActorComponent->IsActive())
	{
		UE_LOG(LogTemp, Warning, TEXT("FlyComponent is active in %s"), *GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FlyComponent is NOT active in %s"), *GetName());
		MyFlyActorComponent->SetActive(true);  // Ȱ��ȭ ����
	}
}

// Called every frame
void APSH_BlockActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

// 	if (bGrab) // true �ϋ�
// 	{
// 		FVector worldLoc; // ���콺�� ���� ������
// 		FVector worldDir; // ī�޶� �����ǰ� ���콺 Ŭ�� ��Ұ��� ����
// 
// 		APlayerController* pc = Cast<APlayerController>(GetOwner());
// 		UE_LOG(LogTemp, Warning, TEXT("%s"), *GetOwner()->GetName());
// 
// 		if (pc == nullptr) return;
// 		pc->DeprojectMousePositionToWorld(worldLoc, worldDir);
// 
// 		/*UE_LOG(LogTemp, Warning, TEXT("X : %f Y : %f Z : %f"), worldLoc.X, worldLoc.Y, worldLoc.Z);*/
// 		FVector TargetLocation = worldLoc + (worldDir * 200.0f); // �ణ �������� �̵�
// 
// 		playerHandle->SetTargetLocation(TargetLocation);
// 	}
}
void APSH_BlockActor::PickUp(class UPhysicsHandleComponent* handle)
{
	if (handle == nullptr) return;
	
	// �θ���� ���� ����
	Remove();

	// ��� ���
	handle->GrabComponentAtLocationWithRotation(meshComp, NAME_None, GetActorLocation(), GetActorRotation());

	pickedUp = true;

	meshComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	meshComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);

	for (auto* actor : childsActors)
	{
		Cast<APSH_BlockActor>(actor)->ChildCollisionUpdate(ECollisionEnabled::QueryOnly);
	}
}

void APSH_BlockActor::Drop(class UPhysicsHandleComponent* physicshandle)
{
	if (physicshandle != nullptr)
	{
		physicshandle->ReleaseComponent();
	}
	
	pickedUp = false;

	meshComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	meshComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	meshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	for (auto* actor : childsActors)
	{
		Cast<APSH_BlockActor>(actor)->ChildCollisionUpdate(ECollisionEnabled::QueryAndPhysics);
	}
}

void APSH_BlockActor::Place(class APSH_BlockActor* attachActor, FTransform worldTransform)
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Pickup Dev")));

	
	attachActor->AddChild(this); // �θ� ��Ͽ� �ڽ� ������� �߰�

	FAttachmentTransformRules rule = FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
	// �θ� �ꎔ�� ���̱�
	this->AttachToActor(attachActor, rule);

	meshComp->SetSimulatePhysics(false);
	parent = attachActor;

	// �ڽ� ����� ��ġ�� ������ ����
	SetActorRelativeLocation(worldTransform.GetLocation());
	SetActorRotation(worldTransform.GetRotation());

	MyLocation = worldTransform;
	// �θ��Ͽ� ���� �ڽ� ��ϵ� ����
	attachActor->TransferChildren(childsActors);

	meshComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	meshComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	meshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	Tags.Remove(FName("owner"));
	
	for (auto* actor : childsActors)
	{
		Cast<APSH_BlockActor>(actor)->ChildCollisionUpdate(ECollisionEnabled::QueryAndPhysics);
	}
}

void APSH_BlockActor::Remove()
{

	if(parent == nullptr) return;
	
	// �θ𿡼� �и�
	FDetachmentTransformRules rule = FDetachmentTransformRules::KeepWorldTransform;
	DetachFromActor(rule);

	// �ù����̼� Ȱ��ȭ
	meshComp->SetSimulatePhysics(true);

	// �θ𿡼� �ڽ� ����
	parent->RemoveChild(this);
	parent->RemoveChildren(childsActors);

	Tags.Add(FName("owner"));

	parent = nullptr;
}

void APSH_BlockActor::RemoveChild(class APSH_BlockActor* actor)
{
	if (actor == nullptr) return;
	//�ڽ� ��Ͽ��� ����
	if (childsActors.Contains(actor))  // �ڽ��� ������ ���� ����
	{
		childsActors.Remove(actor);
	}
}

void APSH_BlockActor::RemoveChildren(TArray<AActor*> childActor)
{
	if (childActor.IsEmpty()) return; // �迭�� ��������� �˻�x

	for (auto* actor : childActor)
	{
		if (Cast<APSH_BlockActor>(actor))
		{
			RemoveChild(Cast<APSH_BlockActor>(actor));
		}
	}
}

void APSH_BlockActor::ChildCollisionUpdate(ECollisionEnabled::Type NewType) // �ڽ� �ݸ��� ������Ʈ
{
	meshComp->SetCollisionEnabled(NewType);

	ECollisionResponse newResponse = ECR_Ignore;

	switch (NewType)
	{
	case ECollisionEnabled::NoCollision:
		newResponse = ECR_Overlap;
		pickedUp = true;
		break;
	case ECollisionEnabled::QueryOnly:
		newResponse = ECR_Overlap;
		pickedUp = true;
		break;
	case ECollisionEnabled::PhysicsOnly:
		newResponse = ECR_Ignore;
		pickedUp = false;
		break;
	case ECollisionEnabled::QueryAndPhysics:
		newResponse = ECR_Block;
		pickedUp = false;
		break;
	case ECollisionEnabled::ProbeOnly:
		newResponse = ECR_Ignore;
		pickedUp = false;
		break;
	case ECollisionEnabled::QueryAndProbe:
		newResponse = ECR_Ignore;
		pickedUp = false;
		break;
	}

	meshComp->SetCollisionResponseToChannel(ECC_PhysicsBody, newResponse);
	meshComp->SetCollisionResponseToChannel(ECC_WorldStatic, newResponse);


	for (auto* actor : childsActors)
	{
		Cast<APSH_BlockActor>(actor)->ChildCollisionUpdate(NewType);
	}
}

TArray<FVector> APSH_BlockActor::GetSnapPoints()
{
	return snapPoints;
}
TArray<FRotator> APSH_BlockActor::GetSnapDirections()
{
	return snapDirections;
}
TArray<int32> APSH_BlockActor::GetsnapPritority()
{
	return snapPritority;
}
void APSH_BlockActor::AddSnapPoint(FVector location, FRotator rotation, int32 priority)
{
	snapPoints.Add(location);
	snapDirections.Add(rotation);
	snapPritority.Add(priority);

	SnapApplyPriority();
}

void APSH_BlockActor::SnapApplyPriority()
{
	// Loop Through All Snap Points

	// �ּ� 2���� �켱������ �ִ��� Ȯ��
	if (snapPritority.Num() < 2) return;

	// �ӽ� �迭 ũ�⸦ �̸� ����
	TArray<FVector> tempSnapPoints;
	tempSnapPoints.SetNum(snapPoints.Num());

	TArray<FRotator> tempSnapDirections;
	tempSnapDirections.SetNum(snapDirections.Num());

	TArray<int32> tempSnapPritority;
	tempSnapPritority.SetNum(snapPritority.Num());

	for (int i = 0; i < snapPritority.Num(); i++)
	{
		int32 minIndex = 0;
		int32 pilYoeObSeum = 0;

		// �ּ� �켱���� ã��
		UKismetMathLibrary::MinOfIntArray(snapPritority, minIndex, pilYoeObSeum);

		// �迭 ũ�⸦ �������� ���� (Size to Fit ��� ����)
		if (tempSnapPoints.Num() <= i)
		{
			tempSnapPoints.SetNum(i + 1);  // �迭 ũ�� ���߱�
		}
		if (tempSnapDirections.Num() <= i)
		{
			tempSnapDirections.SetNum(i + 1);  // �迭 ũ�� ���߱�
		}
		if (tempSnapPritority.Num() <= i)
		{
			tempSnapPritority.SetNum(i + 1);  // �迭 ũ�� ���߱�
		}

		// �ּҰ��� �ش��ϴ� ���� ����Ʈ �� ������ �ӽ� �迭�� ����
		if (snapPoints.IsValidIndex(minIndex))
		{
			tempSnapPoints[i] = snapPoints[minIndex];
		}
		if (snapDirections.IsValidIndex(minIndex))
		{
			tempSnapDirections[i] = snapDirections[minIndex];
		}
		if (snapPritority.IsValidIndex(minIndex))
		{
			tempSnapPritority[i] = snapPritority[minIndex];
		}

		// �ּҰ��� �ִ밪���� �����Ͽ� ���� �ݺ����� ����
		if (snapPritority.IsValidIndex(minIndex))
		{
			snapPritority[minIndex] = 2147483647;  // int32 �ִ밪
		}
	}

	// �迭 ���Ҵ�
	snapPoints = tempSnapPoints;
	snapDirections = tempSnapDirections;
	snapPritority = tempSnapPritority;
}

bool APSH_BlockActor::OvelapChek()
{
	// ��ġ�� ���͵��� ������ �迭 ����
	TArray<AActor*> OutOverlappingActors;

	// ���͸��� Ŭ������ Ÿ���� ���� ��� ���� �˻�
	TSubclassOf<AActor> ClassFilter = AActor::StaticClass();

	// ���� �޽� ������Ʈ�� ��ġ�� ��� ���͸� ã��
	meshComp->GetOverlappingActors(OutOverlappingActors, ClassFilter);

	// ��ġ�� ���Ͱ� ������ ��ȿ�� ��ġ�� ����
	 validPlacement = OutOverlappingActors.Num() == 0;

	// �ڽ� ���͵鿡 ���� ��������� ��ħ �˻�
	for (auto* actor : childsActors)
	{
		if (actor == nullptr)
		{
			continue;  // Null �����Ͱ� ���� ��� �ǳʶ�
		}

		APSH_BlockActor* target = Cast<APSH_BlockActor>(actor);

		if (target)
		{
			if (target->OvelapChek()) // true�� ������ true�� ������ ���ɼ��� �������� false�� �ٷ� ���� �Ҵ�.
			{
				validPlacement = false;
			}
		}
	}

	//return validPlacement; // ���� ��� ��ȯ
	return true; // �Ž� ũ���� ������ ������ ������ true ��ȯ, �Ŀ� ����.
}


void APSH_BlockActor::AddChild(class APSH_BlockActor* childActor)
{
	//�ڽſ��� �ڽ��� �߰�
	if (!childsActors.Contains(childActor)) // �ߺ� �߰� ����
	{
		childsActors.Add(childActor);
		childActor->parent = this;  // �ڽ��� �θ� ����
	}
}


void APSH_BlockActor::TransferChildren(TArray<AActor*> childActor)
{
	for (auto* actor : childActor)
	{
		if (Cast<APSH_BlockActor>(actor))
		{
			AddChild(Cast<APSH_BlockActor>(actor));
		}
	}
}

void APSH_BlockActor::OnComponentSleep(UPrimitiveComponent* SleepingComponent, FName BoneName)
{
		// ���� ������Ʈ�� ����� (Wake Rigid Body)
		SleepingComponent->WakeRigidBody(BoneName);

}

FPSH_ObjectData APSH_BlockActor::SaveBlockHierachy()
{

	FPSH_ObjectData Data;

	// �θ� ���� Ŭ���� �� ��ġ ������ ����
	Data.actor = GetClass();

	FTransform actorTransfrom = GetActorTransform();
	
	actorTransfrom.SetLocation(
	FVector(actorTransfrom.GetLocation().X, actorTransfrom.GetLocation().Y, FMath::RoundToFloat(actorTransfrom.GetLocation().Z / 50.0f) * 50.0f)
	);

	Data.actorTransfrom = actorTransfrom;

	// �ڽ� ������ �ִ��� Ȯ��
	if (childsActors.Num() > 0)
	{
		FPSH_Childdats ChildrenData = SaveBlock();  // �ڽ� �� ������ �����ϴ� �Լ� ȣ��
		Data.childsData.Add(ChildrenData);  // ������ �ڽ� �� ������ Data�� �߰�
	}

	return Data;
}

FPSH_Childdats  APSH_BlockActor::SaveBlock()
{ 

	FPSH_Childdats ChildrenData;

	for (auto* Child : childsActors)
	{
		APSH_BlockActor* ChildBlock = Cast<APSH_BlockActor>(Child);
		if (ChildBlock)
		{
			// �� �ڽ� ���� ������ ����
			FPSH_ChildData SingleChildData;
			SingleChildData.actor = ChildBlock->GetClass();
			SingleChildData.actorTransfrom = ChildBlock->GetActorTransform();
			ChildrenData.childData.Add(SingleChildData);  // �ڽ� ���� �߰�

			// �ڽ��� �ڽ� ���鵵 ��������� ����
			if (ChildBlock->childsActors.Num() > 0)
			{
				FPSH_Childdats SubChildrenData = ChildBlock->SaveBlock();  // �ڽ� ���� �ڽĵ鵵 ����
				ChildrenData.childData.Append(SubChildrenData.childData);  // �ڽ��� �ڽ� �����͸� �߰�
			}
		}
	}

	return ChildrenData;

}

void APSH_BlockActor::LoadBlockHierarchy(const FPSH_ObjectData& Data)
{
	
	SetActorTransform(Data.actorTransfrom);

	// �ڽ� ���� ���� �� �ҷ�����
	for (const FPSH_Childdats& ChildrenData : Data.childsData)
	{
		for (const FPSH_ChildData& ChildData : ChildrenData.childData)
		{
			if (ChildData.actor)
			{
				APSH_BlockActor* ChildBlock = GetWorld()->SpawnActor<APSH_BlockActor>(
					ChildData.actor,
					ChildData.actorTransfrom
				);

				if (ChildBlock)
				{
					// �θ� ���� �ڽ� ���� ����
					ChildBlock->meshComp->SetSimulatePhysics(false);
					ChildBlock->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
					AddChild(ChildBlock);

					// �ڽ��� �ڽ� ���� ��������� �ҷ���
					ChildBlock->LoadBlockHierarchy(ChildBlock->SaveBlockHierachy());
				}
			}
		}
	}

}
void APSH_BlockActor::AllDestroy()
{
	for (auto* actor : childsActors)
	{
		APSH_BlockActor* ChildBlock = Cast<APSH_BlockActor>(actor);

		if (ChildBlock->childsActors.Num() >= 0)
		{
			ChildBlock->AllDestroy();
		}
	}
	Destroy();
}

void APSH_BlockActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

