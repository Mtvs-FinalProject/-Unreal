// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PSH/PSH_DataTable/PSH_MechDataTable.h"
#include "MyMoveActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FINALPROJECT_API UMyMoveActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyMoveActorComponent();

	// 이동속도 변수
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed;

	// 이동방향 변수
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Movement")
	FVector MoveDirection;

	// 이동 상태 변수
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Movement")
	bool bShouldMove;

	// 이동 시작 위치
	FVector StartLocation;

	// 이동거리 변수
	float MoveDistance;

	// 최대 이동거리 변수
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Movement")
	float MaxDistance;	

	// 왕복 방향 반전 플래그
	bool bReverseDirection;

	// 무한 왕복 모드(check box 사용 시 true)
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Movement")
	bool bLoopMode;

	// 왕복 횟수 설정(체크박스가 비활성화된 경우 사용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	int32 LoopCount;

	// 단순 이동 모드
    UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Movement")
    bool bSingleDirection;

	int32 CurrentLoop;

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

	UFUNCTION()
	void GetDelegateBool(bool delegatebool);

	FPSH_FunctionBlockData SaveData();

	void LoadData(FPSH_FunctionBlockData funtionData);
	
	UFUNCTION(Server,Unreliable)
	void SRPC_SetOwnerLocation(const FVector & newLocation);


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
