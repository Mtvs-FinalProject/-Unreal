// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyFlyActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FINALPROJECT_API UMyFlyActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyFlyActorComponent();

	// 상승,하강 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fly")
	float FlySpeed;

	// 상,하강 방향
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fly")
	FVector FlyDirection;

	// 상승 상태 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fly")
	bool bShouldFly;	

	// 상승 시작 위치
	FVector StartLocation;

	// 상승 거리 변수
	float FlyDistance;

	// 최대 상승 거리
	float MaxFlyDistance;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Fly")
	void ObjectFly(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Fly")
	void StartFly();

	UFUNCTION(BlueprintCallable, Category = "Fly")
	void StopFly();

	UFUNCTION(BlueprintCallable, Category = "Fly")
	void OriginFly();
};
