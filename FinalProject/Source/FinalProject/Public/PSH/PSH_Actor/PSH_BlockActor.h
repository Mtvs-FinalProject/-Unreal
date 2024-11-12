// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../PSH_DataTable/PSH_MechDataTable.h"
#include "PSH_BlockActor.generated.h"

UCLASS()
class FINALPROJECT_API APSH_BlockActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APSH_BlockActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UStaticMeshComponent * meshComp;

	/// 블럭 자식 오브젝트 배열
	UPROPERTY(EditAnywhere)
	TArray<class AActor*> childsActors;

	// 부착 위치
	UPROPERTY(EditAnywhere)
	TArray<FVector> snapPoints;

	UPROPERTY(EditAnywhere)
	TArray<FRotator> snapDirections; // 블럭 지도.?

	UPROPERTY(EditAnywhere)
	TArray<int32> snapPritority; // 블럭 우선사항

	int32 connectionsize =0;

	bool validPlacement;

	bool pickedUp;


	UFUNCTION(NetMulticast,Reliable)
	void MRPC_PickUp(class UPhysicsHandleComponent* handle);
	void PickUp(class UPhysicsHandleComponent* handle);

	void Remove();
	
	UFUNCTION(NetMulticast,Reliable)
	void MRPC_Remove();

	UFUNCTION(Server,Reliable)
	void SRPC_Remove();

	bool bGrab = false;
	
	UPROPERTY(EditAnywhere)
	class APSH_BlockActor * parent;

	TArray<FVector> GetSnapPoints();

	TArray<FRotator> GetSnapDirections();

	TArray<int32> GetsnapPritority();

	void AddSnapPoint(FVector location, FRotator rotation, int32 priority);

	bool OvelapChek();

	void SnapApplyPriority();

	void Drop(class UPhysicsHandleComponent * physicshandle);

	UFUNCTION(NetMulticast,Reliable)
	void MRPC_Drop(class UPhysicsHandleComponent * physicshandle);


	void Place(class APSH_BlockActor* attachActor, FTransform worldTransform);
	
	UFUNCTION(NetMulticast,Reliable)
	void MRPC_Place(class APSH_BlockActor* attachActor, FTransform worldTransform);

	void AddChild(class APSH_BlockActor* childActor);

	void ChildCollisionUpdate(ECollisionEnabled::Type NewType);

	void TransferChildren(TArray<AActor*> childActor);

	void RemoveChild(class APSH_BlockActor * actor);
	void RemoveChildren(TArray<AActor*> childActor);

	UFUNCTION()
	void OnComponentSleep(UPrimitiveComponent* SleepingComponent, FName BoneName);

	FPSH_ObjectData SaveBlockHierachy();

	FPSH_Childdats  SaveBlock(/*FPSH_ObjectData& blockdata*/);

/*	void LoadBlockHierarchy(const FPSH_ObjectData& Data , TSet<APSH_BlockActor*>& ProcessedBlocks);*/
	void LoadBlockHierarchy(const FPSH_ObjectData& Data);

	void AllDestroy();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 특정 기능 컴포넌트 추가
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//class UMyFlyActorComponent* MyFlyActorComponent;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//class UMyMoveActorComponent* MyMoveActorComponent;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//class UMyRotateActorComponent* MyRotateActorComponent;

	// 플레이어 중복잡기 방지를 위해 오너 정해주기 ( Net에서 사용하는 GetSetOwner과 다르게 작동을 위해 직접 생성)
	void SetMaster(class APSH_Player* owner);

	APSH_Player * GetMaster();
	
	UFUNCTION(Server,Unreliable)
	void SRPC_BlockScale(float axis);

	UFUNCTION(NetMulticast,Unreliable)
	void MRPC_BlockScale(FVector scaleVec);

	float blockScale = 1;

	FVector scale = FVector(1.f);

	// 잡았을때 이펙트 생성
	UPROPERTY(EditDefaultsOnly)
	class UMaterial * outLineMat;

	void SetOutLine(bool chek);
	
	UFUNCTION(NetMulticast,Reliable)
	void MRPC_SetOutLine(bool chek);

private:
	class APSH_Player * master = nullptr;

};
