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

	// 회전 속도 (도/초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate")
	float RotateSpeed;

	// 회전 방향
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate")
	FRotator RotateDirection;

	// 회전 상태
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate")
	bool bShouldRot;

	// 초기 회전 값
	FRotator StartRotation;

	// 누적 회전 각도 (360도에 도달하면 한 바퀴 완료)
	float TotalRotationAngle;

	// 최대 회전 각도 (기본값은 360도)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate")
	float MaxRotate;

	// 루프 모드 활성화 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotate")
	bool bLoopMode;

	// 현재 회전 횟수
	int32 RotateNum;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 회전 동작
	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void ObjectRolling(float DeltaTime);

	// 회전 시작
	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void StartRolling();

	// 회전 멈춤
	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void StopRolling();

	// 초기 회전 상태로 복구
	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void OriginRolling();

	float LastLogTime = 0.0f; // 마지막으로 로그가 출력된 시간

	UFUNCTION()
	void GetDelegateBool(bool delegatebool);

	FPSH_FunctionBlockData  SaveData();

	void LoadData(FPSH_FunctionBlockData funtionData);

	UFUNCTION(Server,Unreliable)
	void SetOwnerRotation(const FQuat& newRotation);
};
