// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PSH/PSH_DataTable/PSH_MechDataTable.h"
#include "MyRotateActorComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FINALPROJECT_API UMyRotateActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMyRotateActorComponent();

	// ȸ�� �ӵ� (��/��)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate")
	float RotateSpeed;

	// ȸ�� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate")
	FRotator RotateDirection;

	// ȸ�� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate")
	bool bShouldRot;

	// �ʱ� ȸ�� ��
	FRotator StartRotation;

	// ���� ȸ�� ���� (360���� �����ϸ� �� ���� �Ϸ�)
	float TotalRotationAngle;

	// �ִ� ȸ�� ���� (�⺻���� 360��)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate")
	float MaxRotate;

	// ���� ��� Ȱ��ȭ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate")
	bool bLoopMode;

	// ���� ȸ�� Ƚ��
	int32 RotateNum;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ȸ�� ����
	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void ObjectRolling(float DeltaTime);

	// ȸ�� ����
	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void StartRolling();

	// ȸ�� ����
	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void StopRolling();

	// �ʱ� ȸ�� ���·� ����
	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void OriginRolling();

	float LastLogTime = 0.0f; // ���������� �αװ� ��µ� �ð�

	UFUNCTION()
	void GetDelegateBool(bool delegatebool);

	FPSH_FunctionBlockData  SaveData();

	void LoadData(FPSH_FunctionBlockData funtionData);

	UFUNCTION(Server,Unreliable)
	void SetOwnerRotation(const FQuat& newRotation);
};
