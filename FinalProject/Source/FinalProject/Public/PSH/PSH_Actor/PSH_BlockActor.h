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
	TArray<class AActor*> childsActor;


	void PickUp(class UPhysicsHandleComponent* handle);

	bool bGrab = false;

	class UPhysicsHandleComponent* playerHandle;
};
