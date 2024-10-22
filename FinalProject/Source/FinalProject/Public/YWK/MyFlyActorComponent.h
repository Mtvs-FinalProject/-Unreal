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

	// ���,�ϰ� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fly")
	float FlySpeed;

	// ��,�ϰ� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fly")
	FVector FlyDirection;

	// ��� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fly")
	bool bShouldFly;	

	// ��� ���� ��ġ
	FVector StartLocation;

	// ��� �Ÿ� ����
	float FlyDistance;

	// �ִ� ��� �Ÿ�
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
