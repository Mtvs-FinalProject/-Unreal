// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyMoveActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FINALPROJECT_API UMyMoveActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyMoveActorComponent();

	// 이동속도 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed;

	// 이동방향 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector MoveDirection;

	// 이동 상태 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bShouldMove;

	// 이동 시작 위치
	FVector StartLocation;

	// 이동거리 변수
	float MoveDistance;

	// 최대 이동거리 변수
	float MaxDistance;	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ObjectMove(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StartMoving();
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopMoving();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void OriginMove();
};
