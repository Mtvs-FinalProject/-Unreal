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

	// �̵��ӵ� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed;

	// �̵����� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector MoveDirection;

	// �̵� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bShouldMove;

	// �̵� ���� ��ġ
	FVector StartLocation;

	// �̵��Ÿ� ����
	float MoveDistance;

	// �ִ� �̵��Ÿ� ����
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
