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

	// �̵��ӵ� ����
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed;

	// �̵����� ����
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Movement")
	FVector MoveDirection;

	// �̵� ���� ����
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Movement")
	bool bShouldMove;

	// �̵� ���� ��ġ
	FVector StartLocation;

	// �̵��Ÿ� ����
	float MoveDistance;

	// �ִ� �̵��Ÿ� ����
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Movement")
	float MaxDistance;	

	// �պ� ���� ���� �÷���
	bool bReverseDirection;

	// ���� �պ� ���(check box ��� �� true)
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Movement")
	bool bLoopMode;

	// �պ� Ƚ�� ����(üũ�ڽ��� ��Ȱ��ȭ�� ��� ���)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	int32 LoopCount;

	// �ܼ� �̵� ���
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
