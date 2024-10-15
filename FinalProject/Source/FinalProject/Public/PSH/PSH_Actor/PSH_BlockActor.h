// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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


	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent * meshComp;

	/// 블럭 자식 오브젝트 배열
	TArray<class AActor*> childsActors;

	// 부착 위치
	UPROPERTY(EditAnywhere)
	TArray<FVector> snapPoints;

	UPROPERTY(EditAnywhere)
	TArray<FRotator> snapDirections; // 블럭 지도.?

	UPROPERTY(EditAnywhere)
	TArray<int32> snapPritority; // 블럭 우선사항

	int32 connectionsize =0;

	bool pickedUp;

	void PickUp(class UPhysicsHandleComponent* handle);

	void Remove();

	bool bGrab = false;

	class APSH_BlockActor * parent;

	TArray<FVector> GetSnapPoints();

	TArray<FRotator> GetSnapDirections();

	TArray<int32> GetsnapPritority();

	void AddSnapPoint(FVector location, FRotator rotation, int32 priority);

	bool OvelapChek();

	void SnapApplyPriority();

	void Drop(class UPhysicsHandleComponent * physicshandle);

	void Place(class APSH_BlockActor* attachActor, FTransform worldTransform);


	void AddChild(class APSH_BlockActor* childActor);

	void ChildCollisionUpdate(ECollisionEnabled::Type NewType);

	void TransferChildren(TArray<AActor*> childActor);

	void RemoveChild(class APSH_BlockActor * actor);
	void RemoveChildren(TArray<AActor*> childActor);

	UFUNCTION()
	void OnComponentSleep(UPrimitiveComponent* SleepingComponent, FName BoneName);

};
