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
#include "GameFramework/RotatingMovementComponent.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"
#include "CSR/DedicatedServer/AutoRoomLevelInstance.h"

// Sets default values
APSH_BlockActor::APSH_BlockActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	meshComp->SetIsReplicated(true);
	meshComp->SetNotifyRigidBodyCollision(true);

	
	SetRootComponent(meshComp);

	bReplicates = true;
	SetReplicateMovement(true);

	ConstructorHelpers::FObjectFinder<UMaterial> tempOutLine(TEXT("/Script/Engine.Material'/Game/YWK/Effect/Mt_Outline.Mt_Outline'"));

	if (tempOutLine.Succeeded())
	{
		outLineMat = tempOutLine.Object;
	}

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> spawnNiagara(TEXT("/Script/Niagara.NiagaraSystem'/Game/YWK/Effect/MS_Spawn.MS_Spawn'"));
	if (spawnNiagara.Succeeded())
	{
		spawnEffect = spawnNiagara.Object;
	}

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> placeNiagara(TEXT("/Script/Niagara.NiagaraSystem'/Game/YWK/Effect/MS_Star.MS_Star'"));
	if (placeNiagara.Succeeded())
	{
		PlaceEffect = placeNiagara.Object;
	}
	
}

// Called when the game starts or when spawned
void APSH_BlockActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (mapBlock)
	{
		meshComp->SetSimulatePhysics(false);
	}

	if (HasAuthority())
	{
		meshComp->OnComponentHit.AddDynamic(this, &APSH_BlockActor::OnComponentHit);
		APSH_GameModeBase* GM = Cast<APSH_GameModeBase>(GetWorld()->GetAuthGameMode());

		if (GM)
		{
			GM->onStartBlock.AddDynamic(this, &APSH_BlockActor::StartBlockDelgate);
		}
	}
	
}

// Called every frame
void APSH_BlockActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APSH_BlockActor::MRPC_PickUp_Implementation(class UPhysicsHandleComponent* handle)
{
	if (meshComp->IsSimulatingPhysics() == false)
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
	bHit = true;
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
	if (!IsValid(attachActor))
	{
		PRINTLOG(TEXT("Place failed: Parent actor not valid for %s"), *GetName());
		return;
	}
	//MRPC_LoadSetting();
	MRPC_Place(attachActor,worldTransform);
}

void APSH_BlockActor::MRPC_Place_Implementation(class APSH_BlockActor* attachActor, FTransform worldTransform)
{

	PRINTLOG(TEXT("MRC_Place AttachActor return"));
	if (!attachActor)
	{
		return;
	}
	else
	{
		PRINTLOG(TEXT("AttachAcotr : %s"), *attachActor->GetName());
	}

	PRINTLOG(TEXT("MRC_Place AttachActor"));
	if (PlaceEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PlaceEffect, GetActorLocation());
	}

	// 중요: 배치 시 월드 트랜스폼 저장
	placedWorldTransform = worldTransform;
	bIsPlaced = true;

	meshComp->SetSimulatePhysics(false);

	parent = attachActor;
	attachActor->AddChild(this); // 부모 블록에 자식 블록으로 추가

	// 자식 블록의 위치와 방향을 변경
	FAttachmentTransformRules rule = FAttachmentTransformRules(
		EAttachmentRule::KeepWorld,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepWorld,
		true
	);
	// 부모 블럭에 붙이기
	AttachToActor(attachActor, rule);

	//SetActorRelativeTransform(worldTransform);

	SetActorRelativeLocation(worldTransform.GetLocation());
	SetActorRotation(worldTransform.GetRotation());

	// 부모블록에 나의 자식 블록들 전송
	attachActor->TransferChildren(childsActors);


	meshComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	meshComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	meshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	Tags.Remove(FName("owner"));

	for (auto* child : childsActors)
	{
		if (auto* childBlock = Cast<APSH_BlockActor>(child))
		{
			childBlock->ChildCollisionUpdate(ECollisionEnabled::QueryAndPhysics);
		}
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


	if(childsActors.IsEmpty()) return;

	for (auto* actor : childsActors)
	{
		if (APSH_BlockActor* childBlock = Cast<APSH_BlockActor>(actor))
		{
			childBlock->ChildCollisionUpdate(NewType);
		}
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

FPSH_ObjectData APSH_BlockActor::SaveBlock() // 클라 서버 2번 불림
{
	FPSH_ObjectData Data;

	if (locationData.bisSave)
	{
		PRINTLOG(TEXT("locationData.bisSave"));
		return locationData;
	}
	
	// 부모 저장

	if (bIsPlaced)
	{
		Data.blockData.actorTransform = placedWorldTransform;
	}
	else
	{
		Data.blockData.actor = GetClass(); // 블루프린트 사용한것 저장

		FTransform actorTransfrom = GetActorTransform();
		actorTransfrom.SetLocation(FVector(
			actorTransfrom.GetLocation().X,
			actorTransfrom.GetLocation().Y,
			FMath::RoundToFloat(actorTransfrom.GetLocation().Z / 50.0f) * 50.0f
		));
		// 배치되지 않았다면 기존 로직 유지
		Data.blockData.actorTransform = actorTransfrom;
	}

	if (blockDataType != EBlockDataType::NOMAL) // 기본 블럭이 아니라면
	{
		Data.blockData.funcitonData = ComponentSaveData(blockDataType);
	}

	if(childsActors.IsEmpty()) return Data;

	// 자식 블럭 반복.
	for (AActor* actor : childsActors)
	{
		APSH_BlockActor * block = Cast<APSH_BlockActor>(actor);
		PRINTLOG(TEXT("childsActors : %d "), childsActors.Num());
		
		if (block) // 자식 블럭.
		{
			// 자식 데이터 저장.
			Data.blockData.childData.Add(block->SaveChildBlock());
		}
	}

	return Data;

}

FPSH_BlockData APSH_BlockActor::SaveChildBlock() // 서버 클라
{
	FPSH_BlockData data;

	// 내 정보 저장.
	data.actor = GetClass();

	if (bIsPlaced)
	{
		data.actorTransform = placedWorldTransform;
	}
	else
	{
		data.actor = GetClass(); // 블루프린트 사용한것 저장

		FTransform actorTransfrom = GetActorTransform();
		actorTransfrom.SetLocation(FVector(
			actorTransfrom.GetLocation().X,
			actorTransfrom.GetLocation().Y,
			FMath::RoundToFloat(actorTransfrom.GetLocation().Z / 50.0f) * 50.0f
		));
		// 배치되지 않았다면 기존 로직 유지
		data.actorTransform = actorTransfrom;
	}

	if (blockDataType != EBlockDataType::NOMAL) // 기본 블럭이 아니라면
	{
		data.funcitonData = ComponentSaveData(blockDataType);
	}
	// 비어있을경우
	if (childsActors.IsEmpty()) return data;

	for (AActor* actor : childsActors)
	{
		APSH_BlockActor* block = Cast<APSH_BlockActor>(actor);

		if (block) // 자식 블럭.
		{
			PRINTLOG(TEXT("childsActors : %d "), childsActors.Num());
			data.childData.Add(block->SaveChildBlock());
		}
	}
	
	return data;
}

void APSH_BlockActor::SaveBlockLocations() // 클라이언트만.
{
	PRINTLOG(TEXT("SaveBlockLocations"));
	locationData.bisSave = true;

	// 부모 저장
	locationData.blockData.actor = GetClass(); // 블루프린트 사용한것 저장

	FTransform actorTransfrom = GetActorTransform();
	actorTransfrom.SetLocation(FVector(
		actorTransfrom.GetLocation().X,
		actorTransfrom.GetLocation().Y,
		FMath::RoundToFloat(actorTransfrom.GetLocation().Z / 50.0f) * 50.0f
	));
	

	locationData.blockData.actorTransform = actorTransfrom; // 위치 저장

	if (blockDataType != EBlockDataType::NOMAL) // 기본 블럭이 아니라면
	{
		locationData.blockData.funcitonData = ComponentSaveData(blockDataType);
	}

	if (childsActors.IsEmpty()) return;

	// 자식 블럭 반복.
	for (AActor* actor : childsActors)
	{
		APSH_BlockActor* block = Cast<APSH_BlockActor>(actor);

		if (block) // 자식 블럭.
		{
			// 자식 데이터 저장.
			locationData.blockData.childData.Add(block->SaveChildBlock());
		}
	}

}

void APSH_BlockActor::LoadBlockHierarchy(const FPSH_BlockData& data)
{
	if(HasAuthority() == false) return;

	if(data.childData.IsEmpty()) return;

	for (const FPSH_BlockData& childData : data.childData)
	{
		if (childData.actor)
		{
			FActorSpawnParameters SpawnParams;
	
			APSH_BlockActor* childBlock = GetWorld()->SpawnActor<APSH_BlockActor>(
				childData.actor,
				childData.actorTransform,
				SpawnParams
			);

			if (childBlock)
			{
				// Place를 사용해 부모-자식 관계 설정
				childBlock->SetOwner(GetOwner());
				childBlock->Place(this, childData.actorTransform);

				// 컴포넌트 데이터 로드
				if (!childData.funcitonData.IsEmpty())
				{
					childBlock->ComponentLoadData(childData.funcitonData);
				}

				// 재귀적으로 자식 블록 로드
				childBlock->LoadBlockHierarchy(childData);
			}
		}
	}
}

void APSH_BlockActor::MRPC_LoadSetting_Implementation()
{
	meshComp->SetSimulatePhysics(false);
	Tags.Remove(FName("owner"));
}

void APSH_BlockActor::OnRep_SimulatePhysics()
{
	// 클라이언트에서 물리 상태 동기화
	if (meshComp)
	{
		meshComp->SetSimulatePhysics(bSimulatePhysics);
		PRINTLOG(TEXT("OnRep_SimulatePhysics: %s for %s"), bSimulatePhysics ? TEXT("True") : TEXT("False"), *GetName());
	}
}
void APSH_BlockActor::SetSimulatePhysics(bool bEnabled)
{
	if (HasAuthority()) // 서버에서만 상태를 변경
	{
		bSimulatePhysics = bEnabled;
		meshComp->SetSimulatePhysics(bEnabled);
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
	MRPC_StartBlockDelgate(createMode);
}
void APSH_BlockActor::MRPC_StartBlockDelgate_Implementation(bool createMode)
{
	if (componentCreateBoolDelegate.IsBound())
	{
		componentCreateBoolDelegate.Broadcast(createMode);
	}
}

TArray<FPSH_FunctionBlockData> APSH_BlockActor::ComponentSaveData(EBlockDataType dataType)
{
	TArray<FPSH_FunctionBlockData> funcionBlockData;

	switch (blockDataType)
	{
	case EBlockDataType::MOVEANDFLY:
		flyComp = Cast<UMyFlyActorComponent>(GetComponentByClass(UMyFlyActorComponent::StaticClass()));
		moveComp = Cast<UMyMoveActorComponent>(GetComponentByClass(UMyMoveActorComponent::StaticClass()));
		if(flyComp == nullptr || moveComp == nullptr) return funcionBlockData;
		funcionBlockData.Add(moveComp->SaveData());
		funcionBlockData.Add(flyComp->SaveData());
		break;
	case EBlockDataType::ROTATE:
		rotationMovementComp = Cast<UMyRotateActorComponent>(GetComponentByClass(UMyRotateActorComponent::StaticClass()));
		if (rotationMovementComp == nullptr) return funcionBlockData;
		funcionBlockData.Add(rotationMovementComp->SaveData());
		break;
	default :
		PRINTLOG(TEXT("TypeNull"));
		break;
	}

	return funcionBlockData;
}

void APSH_BlockActor::ComponentLoadData(TArray<FPSH_FunctionBlockData> funcionBlockData)
{

	switch (blockDataType)
	{
	case EBlockDataType::MOVEANDFLY:
		flyComp = Cast<UMyFlyActorComponent>(GetComponentByClass(UMyFlyActorComponent::StaticClass()));
		moveComp = Cast<UMyMoveActorComponent>(GetComponentByClass(UMyMoveActorComponent::StaticClass()));
		if (flyComp == nullptr || moveComp == nullptr) return;
		moveComp->LoadData(funcionBlockData[0]);
		flyComp->LoadData(funcionBlockData[1]);
		break;
	case EBlockDataType::ROTATE:
		rotationMovementComp = Cast<UMyRotateActorComponent>(GetComponentByClass(UMyRotateActorComponent::StaticClass()));
		if (rotationMovementComp == nullptr) return;
		rotationMovementComp->LoadData(funcionBlockData[0]);
		break;
	default:
		PRINTLOG(TEXT("TypeNull"));
		break;
	}
}

void APSH_BlockActor::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
	if(HasAuthority() == false) return;

	if(bHit == false || mapBlock) return;
	APSH_BlockActor * mapObject = Cast<APSH_BlockActor>(OtherActor);

	if (mapObject && mapObject->mapBlock && bHit)
	{
		PRINTLOG(TEXT("Hit"));
		MRPC_SpawnEffect(Hit.ImpactPoint);
		bHit = false;
	}

}
void APSH_BlockActor::MRPC_SpawnEffect_Implementation(const FVector & impactPoint)
{
	if(spawnEffect == nullptr) return;
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), spawnEffect, impactPoint);
}
void APSH_BlockActor::SRPC_SetSimulatePhysics_Implementation(bool check)
{
	MRPC_SetSimulatePhysics(check);
	meshComp->SetSimulatePhysics(check);
}
void APSH_BlockActor::MRPC_SetSimulatePhysics_Implementation(bool check)
{
	meshComp->SetSimulatePhysics(check);
}
// 
// bool APSH_BlockActor::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
// {
// 
// 	// 레벨 인스턴스가 소유한 액터인 경우
// 	if (bIsOwnedByRoomInstance)
// 	{
// 		// 뷰 타겟의 컨트롤러 획득
// 		AController* TargetController = Cast<AController>(ViewTarget->GetInstigatorController());
// 		if (!TargetController)
// 		{
// 			if (const APawn* TargetPawn = Cast<APawn>(ViewTarget))
// 			{
// 				TargetController = TargetPawn->GetController();
// 			}
// 		}
// 		else
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("ViewTarget is null in IsNetRelevantFor"));
// 			//return false;
// 		}
// 
// 		// 플레이어 컨트롤러인 경우
// 		if (APlayerController* PC = Cast<APlayerController>(TargetController))
// 		{
// 			if (GetOwner())
// 			{
// 				if (AAutoRoomLevelInstance* OwningRoom = Cast<AAutoRoomLevelInstance>(GetOwner()))
// 				{
// 					return OwningRoom->IsPlayerInRoom(PC);
// 				}
// 			}
// 			else
// 			{
// 				UE_LOG(LogTemp, Warning, TEXT("Owner is null for %s in IsNetRelevantFor"), *GetName());
// 			}
// 			return false;
// 		}
// 		return false;
// 	}
// 
// 	return Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
// }
// 
// void APSH_BlockActor::PostInitializeComponents()
// {
// 	Super::PostInitializeComponents();
// 
// 	if (HasAuthority())
// 	{
// 		if (AAutoRoomLevelInstance* RoomInstance = Cast<AAutoRoomLevelInstance>(GetOwner()))
// 		{
// 			bIsOwnedByRoomInstance = true;
// 
// 			// 네트워크 설정 확인
// 			if (!bReplicates)
// 			{
// 				SetReplicates(true);
// 				SetReplicateMovement(true);
// 			}
// 
// 			UE_LOG(LogTemp, Log, TEXT("Block %s initialized with owner %s"),
// 				*GetName(),
// 				*RoomInstance->GetName());
// 		}
// 	}
// }
