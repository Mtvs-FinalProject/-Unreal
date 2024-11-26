// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PSH/PSH_DataTable/PSH_MechDataTable.h"
#include "MyFlyActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FINALPROJECT_API UMyFlyActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyFlyActorComponent();

	// 상승,하강 속도
	UPROPERTY(EditAnywhere, Replicated,BlueprintReadWrite, Category = "Fly")
	float FlySpeed;

	// 상,하강 방향
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Fly" )
	FVector FlyDirection;

	// 상승 상태 변수
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Fly")
	bool bShouldFly;	

	// 상승 시작 위치
	UPROPERTY(EditAnywhere, Category = "Fly")
	FVector StartLocation;

	// 상승 거리 변수
	UPROPERTY(EditAnywhere, Category = "Fly")
	float FlyDistance;

	// 최대 상승 거리
	UPROPERTY(EditAnywhere, Category = "Fly")
	float MaxFlyDistance;

	// 무한 왕복 모드
	UPROPERTY(EditAnywhere, Replicated,BlueprintReadWrite, Category = "Fly")
	bool bLoopMode;

	// 단순 이동 모드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fly")
	bool bSingleDirection;

	// 왕복 횟수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fly")
    int32 LoopCount;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void objectFly(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Fly")
	void StartFly();

	UFUNCTION(BlueprintCallable, Category = "Fly")
	void StopFly();

	UFUNCTION(BlueprintCallable, Category = "Fly")
	void OriginFly();

	UFUNCTION()
	void GetDelegateBool(bool delegatebool);

	FPSH_FunctionBlockData SaveData();

	void LoadData(FPSH_FunctionBlockData funtionData);

	UFUNCTION(Server,Unreliable)
	void SRPC_SetOwnerLocation(const FVector & newLocation);

};
