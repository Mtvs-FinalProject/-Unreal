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

	// ȸ���ӵ� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate")
	float RotateSpeed;

	// ȸ������ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate")
	FRotator RotateDirection;

	// ȸ�� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate")
	bool bShouldRot;

	// ȸ�� ���� ��
	FRotator StartRotation;

	// ȸ�� Ƚ�� ����
	int32 RotateNum;

	// �ִ� ȸ�� ����
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
