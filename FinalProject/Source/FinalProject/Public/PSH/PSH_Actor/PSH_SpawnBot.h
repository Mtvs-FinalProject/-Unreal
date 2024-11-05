// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PSH_SpawnBot.generated.h"

UENUM(BlueprintType)
enum class EspawnState : uint8
{
	IDLE , // 대기
	SPAWNMOVE,
	IDLEMOVE,
	SPAWN, // 스폰 가능
};

UCLASS()
class FINALPROJECT_API APSH_SpawnBot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APSH_SpawnBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
// 
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent * compMesh;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* downMesh;

	UPROPERTY(EditAnywhere)
	class USpotLightComponent * spotLight;
	
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent * lightMesh;

	UPROPERTY(EditDefaultsOnly , Category = "Mat")
	TArray<class UMaterial*> matArray;

	void SetPlayer(class APSH_Player* playerClass);

	void MoveTo();

	void IdleState();
	void SpawnMoveState(const float & deltaTime);
	void IdleMoveState(const float & deltaTime);
	void SpawnState(const float & deltaTime);

	
	void SetState(EspawnState State);

	EspawnState state = EspawnState::IDLE;

private:

	void LineChek();

	float idleTime;
	UPROPERTY()
	class APSH_Player * player;

	FVector TagetLoc;

};
