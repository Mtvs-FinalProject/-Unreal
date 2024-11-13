// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/PSH_Actor/PSH_BlockActor.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetArrayLibrary.h"
#include "PSH/PSH_Player/PSH_Player.h"
#include "Net/UnrealNetwork.h"
#include "../FinalProject.h"
#include "YWK/MyMoveActorComponent.h"
#include "YWK/MyFlyActorComponent.h"
#include "YWK/MyRotateActorComponent.h"
#include "PSH/PSH_GameMode/PSH_GameModeBase.h"

// Sets default values
APSH_BlockActor::APSH_BlockActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	meshComp->SetIsReplicated(true);
	
	SetRootComponent(meshComp);

	bReplicates = true;
	SetReplicateMovement(true);

	ConstructorHelpers::FObjectFinder<UMaterial> tempOutLine(TEXT("/Script/Engine.Material'/Game/YWK/Effect/Mt_Outline.Mt_Outline'"));

	if (tempOutLine.Succeeded())
	{
		outLineMat = tempOutLine.Object;
	}
	// 기능 컴포넌트들
	//MyMoveActorComponent = CreateDefaultSubobject<UMyMoveActorComponent>(TEXT("MoveComponent"));
	//MyFlyActorComponent = CreateDefaultSubobject<UMyFlyActorComponent>(TEXT("FlyComponent"));
	//MyRotateActorComponent = CreateDefaultSubobject<UMyRotateActorComponent>(TEXT("RotateComponent"));

	//// 컴포넌트가 특정 상황에서만 활성화되도록 설정
	//MyMoveActorComponent->SetActive(false);
	//MyFlyActorComponent->SetActive(false);
	//MyRotateActorComponent->SetActive(false);
}

// Called when the game starts or when spawned
void APSH_BlockActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (mapBlock)
	{
		meshComp->SetSimulatePhysics(false);
	}
	else
	{
		meshComp->SetSimulatePhysics(true);
	}

	if (HasAuthority())
	{
		APSH_GameModeBase* GM = Cast<APSH_GameModeBase>(GetWorld()->GetAuthGameMode());

		if (GM)
		{
			GM->onStartBlock.AddDynamic(this, &APSH_BlockActor::StartBlockDelgate);
			PRINTLOG(TEXT("StartBlockDelgate"));
		}
	}
	/*meshComp->OnComponentSleep.AddDynamic(this, &APSH_BlockActor::OnComponentSleep);*/

	//if (MyMoveActorComponent && MyMoveActorComponent->IsComponentTickEnabled())
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("MoveComponent is active in %s"), *GetName());
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("MoveComponent is NOT active in %s"), *GetName());
	//	MyMoveActorComponent->SetComponentTickEnabled(true);  // 컴포넌트 활성화 설정
	//}

	//if (MyFlyActorComponent && MyFlyActorComponent->IsActive())
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("FlyComponent is active in %s"), *GetName());
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("FlyComponent is NOT active in %s"), *GetName());
	//	MyFlyActorComponent->SetActive(true);  // 활성화 설정
	//}
}

// Called every frame
void APSH_BlockActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	//PRINTLOG(TEXT("%f,%f,%f"),GetActorRotation(),)
}

void APSH_BlockActor::MRPC_PickUp_Implementation(class UPhysicsHandleComponent* handle)
{
	if (mapBlock)
	{
		meshComp->SetSimulatePhysics(true);
	}

	if (handle == nullptr) return;

	handle->GrabComponentAtLocationWithRotation(meshComp, NAME_None, GetActorLocation(), GetActorRotation());

	meshComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	meshComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	meshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	for (auto* actor : childsActors)
	{
		Cast<APSH_BlockActor>(actor)->ChildCollisionUpdate(ECollisionEnabled::QueryOnly);
	}
}

void APSH_BlockActor::PickUp(class UPhysicsHandleComponent* handle)
{
	if (handle == nullptr) return;
	// 블록 잡기
	MRPC_Remove(); // 항상 서버라서

	pickedUp = true;
	MRPC_PickUp(handle);

}

// srpc
void APSH_BlockActor::Drop(class UPhysicsHandleComponent* physicshandle)
{
	MRPC_Drop(physicshandle);
}

void APSH_BlockActor::MRPC_Drop_Implementation(class UPhysicsHandleComponent* physicshandle)
{
	if (physicshandle != nullptr)
	{
		physicshandle->ReleaseComponent();
	}
	else
	{
		return;
	}
	
	if (mapBlock)
	{
		meshComp->SetSimulatePhysics(false);	
	}

	SetMaster(nullptr);
	pickedUp = false;

	meshComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	meshComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	meshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	meshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	for (auto* actor : childsActors)
	{
		Cast<APSH_BlockActor>(actor)->ChildCollisionUpdate(ECollisionEnabled::QueryAndPhysics);
	}
}
void APSH_BlockActor::Place(class APSH_BlockActor* attachActor, FTransform worldTransform)
{
	MRPC_Place(attachActor,worldTransform);
}

void APSH_BlockActor::MRPC_Place_Implementation(class APSH_BlockActor* attachActor, FTransform worldTransform)
{
	attachActor->AddChild(this); // 부모 블록에 자식 블록으로 추가

	meshComp->SetSimulatePhysics(false);
	parent = attachActor;

	// 자식 블록의 위치와 방향을 변경
	FAttachmentTransformRules rule = FAttachmentTransformRules(
		EAttachmentRule::KeepWorld,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepWorld,
		true
	);
	// 부모 블럭에 붙이기
	AttachToActor(attachActor, rule);

	SetActorRelativeLocation(worldTransform.GetLocation());
	SetActorRotation(worldTransform.GetRotation());

	// 부모블록에 나의 자식 블록들 전송
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
	SRPC_Remove();
}

void APSH_BlockActor::SRPC_Remove_Implementation()
{
	MRPC_Remove();
}

void APSH_BlockActor::MRPC_Remove_Implementation()
{
	if (parent == nullptr) return;

	// 부모에서 분리
	FDetachmentTransformRules rule = FDetachmentTransformRules::KeepWorldTransform;
	DetachFromActor(rule);

	// 시물레이션 활성화

	meshComp->SetSimulatePhysics(true);
	
	// 부모에서 자식 제거
	parent->RemoveChild(this);
	parent->RemoveChildren(childsActors);

	Tags.Add(FName("owner"));

	parent = nullptr;
}

void APSH_BlockActor::RemoveChild(class APSH_BlockActor* actor)
{
	if (actor == nullptr) return;
	//자식 목록에서 제거
	if (childsActors.Contains(actor))  // 자식이 존재할 때만 제거
	{
		childsActors.Remove(actor);
	}
}

void APSH_BlockActor::RemoveChildren(TArray<AActor*> childActor)
{
	if (childActor.IsEmpty()) return; // 배열이 비어있으면 검사x

	for (auto* actor : childActor)
	{
		if (Cast<APSH_BlockActor>(actor))
		{
			RemoveChild(Cast<APSH_BlockActor>(actor));
		}
	}
}

void APSH_BlockActor::ChildCollisionUpdate(ECollisionEnabled::Type NewType) // 자식 콜리전 업데이트
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

	// 최소 2개의 우선순위가 있는지 확인
	if (snapPritority.Num() < 2) return;

	// 임시 배열 크기를 미리 설정
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

		// 최소 우선순위 찾기
		UKismetMathLibrary::MinOfIntArray(snapPritority, minIndex, pilYoeObSeum);

		// 배열 크기를 동적으로 조정 (Size to Fit 기능 구현)
		if (tempSnapPoints.Num() <= i)
		{
			tempSnapPoints.SetNum(i + 1);  // 배열 크기 맞추기
		}
		if (tempSnapDirections.Num() <= i)
		{
			tempSnapDirections.SetNum(i + 1);  // 배열 크기 맞추기
		}
		if (tempSnapPritority.Num() <= i)
		{
			tempSnapPritority.SetNum(i + 1);  // 배열 크기 맞추기
		}

		// 최소값에 해당하는 스냅 포인트 및 방향을 임시 배열에 복사
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

		// 최소값을 최대값으로 설정하여 이후 반복에서 제외
		if (snapPritority.IsValidIndex(minIndex))
		{
			snapPritority[minIndex] = 2147483647;  // int32 최대값
		}
	}

	// 배열 재할당
	snapPoints = tempSnapPoints;
	snapDirections = tempSnapDirections;
	snapPritority = tempSnapPritority;
}

bool APSH_BlockActor::OvelapChek()
{
	// 겹치는 액터들을 저장할 배열 선언
	TArray<AActor*> OutOverlappingActors;

	// 필터링할 클래스의 타입을 정의 모든 엑터 검사
	TSubclassOf<AActor> ClassFilter = AActor::StaticClass();

	// 현재 메쉬 컴포넌트와 겹치는 모든 액터를 찾음
	meshComp->GetOverlappingActors(OutOverlappingActors, ClassFilter);

	// 겹치는 액터가 없으면 유효한 배치로 간주
	 validPlacement = OutOverlappingActors.Num() == 0;

	// 자식 액터들에 대해 재귀적으로 겹침 검사
	for (auto* actor : childsActors)
	{
		if (actor == nullptr)
		{
			continue;  // Null 포인터가 있을 경우 건너뜀
		}

		APSH_BlockActor* target = Cast<APSH_BlockActor>(actor);

		if (target)
		{
			if (target->OvelapChek()) // true가 나오면 true로 씌워질 가능성이 있음으로 false로 바로 변수 할당.
			{
				validPlacement = false;
			}
		}
	}

	//return validPlacement; // 최종 결과 반환
	return true; // 매쉬 크기의 문제로 현제는 무조건 true 반환, 후에 변경.
}


void APSH_BlockActor::AddChild(class APSH_BlockActor* childActor)
{
	//자신에게 자식을 추가
	
	if (!childsActors.Contains(childActor)) // 중복 추가 방지
	{
		childsActors.Add(childActor);
		childActor->parent = this;  // 자식의 부모 설정
	}
}


void APSH_BlockActor::TransferChildren(TArray<AActor*> childActor)
{
	PRINTLOG(TEXT("AddChild?"));
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
		// 물리 컴포넌트를 깨우기 (Wake Rigid Body)
	SleepingComponent->WakeRigidBody(BoneName);

}

FPSH_ObjectData APSH_BlockActor::SaveBlockHierachy()
{

	FPSH_ObjectData Data;

	// 부모 블럭의 클래스 및 위치 정보를 저장
	Data.actor = GetClass();

	FTransform actorTransfrom = GetActorTransform();
	
	actorTransfrom.SetLocation(
	FVector(actorTransfrom.GetLocation().X, actorTransfrom.GetLocation().Y, FMath::RoundToFloat(actorTransfrom.GetLocation().Z / 50.0f) * 50.0f)
	);

	Data.actorTransfrom = actorTransfrom;

	// 자식 블럭들이 있는지 확인
	if (childsActors.Num() > 0)
	{
		FPSH_Childdats ChildrenData = SaveBlock();  // 자식 블럭 정보를 저장하는 함수 호출
		Data.childsData.Add(ChildrenData);  // 저장한 자식 블럭 정보를 Data에 추가
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
			// 각 자식 블럭의 정보를 저장
			FPSH_ChildData SingleChildData;
			SingleChildData.actor = ChildBlock->GetClass();
			SingleChildData.actorTransfrom = ChildBlock->GetActorTransform();
			ChildrenData.childData.Add(SingleChildData);  // 자식 정보 추가

			// 자식의 자식 블럭들도 재귀적으로 저장
			if (ChildBlock->childsActors.Num() > 0)
			{
				FPSH_Childdats SubChildrenData = ChildBlock->SaveBlock();  // 자식 블럭의 자식들도 저장
				ChildrenData.childData.Append(SubChildrenData.childData);  // 자식의 자식 데이터를 추가
			}
		}
	}

	return ChildrenData;

}

void APSH_BlockActor::LoadBlockHierarchy(const FPSH_ObjectData& Data)
{
	SetActorTransform(Data.actorTransfrom);

	// 자식 블럭들 생성 및 불러오기
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
					// 부모 블럭에 자식 블럭을 붙임
					ChildBlock->meshComp->SetSimulatePhysics(false);
					ChildBlock->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
					AddChild(ChildBlock);

					// 자식의 자식 블럭도 재귀적으로 불러옴
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

void APSH_BlockActor::SetMaster(class APSH_Player* owner)
{
	master = owner;
}

APSH_Player * APSH_BlockActor::GetMaster()
{
	return master;
}
void APSH_BlockActor::SRPC_BlockScale_Implementation(float axis)
{
	if (axis > 0)
	{
		if (blockScale >= 4) return;

		blockScale += GetWorld()->GetDeltaSeconds() * 10;
	}
	else
	{
		if (blockScale <= 0.5f) return;

		blockScale -= GetWorld()->GetDeltaSeconds() * 10;
	}
	MRPC_BlockScale(scale * blockScale);
}

void APSH_BlockActor::MRPC_BlockScale_Implementation(FVector scaleVec)
{
	SetActorScale3D(scaleVec);
}
void APSH_BlockActor::SetOutLine(bool chek)
{
	MRPC_SetOutLine(chek);
}
void APSH_BlockActor::MRPC_SetOutLine_Implementation(bool chek)
{
	if (chek)
	{
		if (outLineMat == nullptr) return;
		meshComp->SetOverlayMaterial(outLineMat);
	}
	else
	{
		meshComp->SetOverlayMaterial(nullptr);
	}
}

void APSH_BlockActor::StartBlockDelgate(bool createMode)
{
	MROC_StartBlockDelgate(createMode);
}
void APSH_BlockActor::MROC_StartBlockDelgate_Implementation(bool createMode)
{
	if (componentCreateBoolDelegate.IsBound())
	{
		componentCreateBoolDelegate.Broadcast(createMode);
	}
}