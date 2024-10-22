// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyRotateActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FINALPROJECT_API UMyRotateActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyRotateActorComponent();

	// 회전속도 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate")
	float RotateSpeed;

	// 회전방향 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate")
	FRotator RotateDirection;

	// 회전 상태 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate")
	bool bShouldRot;

	// 회전 시작 점
	FRotator StartRotation;

	// 회전 횟수 변수
	int32 RotateNum;

	// 최대 회전 변수
	int32 MaxRotate;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void ObjectRolling(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void StartRolling();

	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void StopRolling();

	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void OriginRolling();
};
