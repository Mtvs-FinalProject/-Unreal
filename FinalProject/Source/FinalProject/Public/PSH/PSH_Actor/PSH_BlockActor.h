// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../PSH_DataTable/PSH_MechDataTable.h"
#include "PSH_BlockActor.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FComponentCreateBoolDelegate, bool);

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

	//��������Ʈ ���
	FComponentCreateBoolDelegate componentCreateBoolDelegate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UStaticMeshComponent * meshComp;

	/// �� �ڽ� ������Ʈ �迭
	UPROPERTY(EditAnywhere)
	TArray<class AActor*> childsActors;

	// ���� ��ġ
	UPROPERTY(EditAnywhere)
	TArray<FVector> snapPoints;

	UPROPERTY(EditAnywhere)
	TArray<FRotator> snapDirections; // �� ����.?

	UPROPERTY(EditAnywhere)
	TArray<int32> snapPritority; // �� �켱����

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

	// Ư�� ��� ������Ʈ �߰�
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//class UMyFlyActorComponent* MyFlyActorComponent;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//class UMyMoveActorComponent* MyMoveActorComponent;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//class UMyRotateActorComponent* MyRotateActorComponent;

	// �÷��̾� �ߺ���� ������ ���� ���� �����ֱ� ( Net���� ����ϴ� GetSetOwner�� �ٸ��� �۵��� ���� ���� ����)
	void SetMaster(class APSH_Player* owner);

	APSH_Player * GetMaster();
	
	UFUNCTION(Server,Unreliable)
	void SRPC_BlockScale(float axis);

	UFUNCTION(NetMulticast,Unreliable)
	void MRPC_BlockScale(FVector scaleVec);

	float blockScale = 1;

	FVector scale = FVector(1.f);

	// ������� ����Ʈ ����
	UPROPERTY(EditDefaultsOnly)
	class UMaterial * outLineMat;

	void SetOutLine(bool chek);
	
	UFUNCTION(NetMulticast,Reliable)
	void MRPC_SetOutLine(bool chek);

	UPROPERTY(EditDefaultsOnly)
	bool mapBlock = false;

// 	UFUNCTION(Server,Reliable)
// 	void SRPC_
	
	
	void ComponentDelegate();

	UFUNCTION()
	void StartBlockDelgate(bool createMode);

	UFUNCTION(NetMulticast,Reliable)
	void MROC_StartBlockDelgate(bool createMode);
private:
	class APSH_Player * master = nullptr;

};
