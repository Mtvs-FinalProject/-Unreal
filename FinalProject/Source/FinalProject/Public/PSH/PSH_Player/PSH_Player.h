// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputActionValue.h"
#include "../PSH_DataTable/PSH_MechDataTable.h"
#include "PSH_Player.generated.h"

UCLASS()
class FINALPROJECT_API APSH_Player : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APSH_Player();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// Input
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext * imc_Player;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TArray<class UInputAction*> inputActions;

	UPROPERTY()
	class UPSH_PlayerAnim * anim;

	//Camera
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent * springArm;

	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent * cameraComp;

	//Components
	UPROPERTY(EditDefaultsOnly)
	class UPhysicsHandleComponent * handleComp; // ���� �����ڵ� ������Ʈ.

	UPROPERTY(EditDefaultsOnly)
	class USceneComponent * rotationHelper; // ȸ�� �����

	UPROPERTY()
	class UCharacterMovementComponent* movementComp;
	
	//PC
	class APSH_PlayerController* pc;
	void InitPcUi();

	// �̵�
	FVector Dir;
	void Move(const FInputActionValue& value);

	void Look(const FInputActionValue& value);

	// �÷��̾� �ൿ
	bool bCreatingMode;

	void PlayerJump();
	void PlayerFly(const FInputActionValue& value);

	bool bFlyTimer;
	bool bFly = false;
	float flyTime = 0.3f;
	float curTime = 0;

	UFUNCTION(Server, Reliable)
	void SRPC_CheckMode();
	UFUNCTION(NetMulticast, Reliable)
	void MRPC_CheckMode(bool check);

	UPROPERTY(EditAnywhere)
	float flayFower = 500.0f;
	
	void PlayerCrouch(const FInputActionValue& value);
	void PlayerUnCrouch(const FInputActionValue& value);

	void PlayerDownFly(const FInputActionValue& value);
	
	UPROPERTY(EditAnywhere)
	class UDataTable * dataTable;

	bool test = false;

	void LoadTest();

	// ����â? â ���� 
	void ShowInterface();

	void OnArtKey(); // art Ȯ��
	
	//������ ����
	void OnBlockScale(const FInputActionValue & value);

	bool bArtKey = false;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ���� UI ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> FirstSelect;

	// ���� ���� �ִ� UI ���� �ν��Ͻ��� ���� (UI ���� �ݱ� ��� ���� ����)
    UPROPERTY()
    UUserWidget* CurrentObjectWidget = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UMaterial * previewMat;

	// ī�޶� ����
	void ToggleARmLength();

	bool bShouldExtend = false;

	// Spawn Bot
	UPROPERTY(EditDefaultsOnly , Category = "Bot")
	TSubclassOf<class APSH_SpawnBot> spawnBotFac;

	UPROPERTY()
	class APSH_SpawnBot * spawnBot;

	UFUNCTION(Server, Reliable)
	void SRPC_SpawnbotIdel();

	UFUNCTION(Server, Reliable)
	void SRPC_SpawnBotMoveTo();

	bool bSpawn = true;
	// garbageBot
	UPROPERTY(EditAnywhere)
	class APSH_GarbageBot* garbagebot;

	UFUNCTION(Server,Reliable)
	void SRPC_GabageBotMovePoint(const FVector & Movelocation);

	UFUNCTION(Server,Reliable)
	void SRPC_GarbageBotInitPoint();

	UFUNCTION(Server, Reliable)
	void SRPC_GarbageBotSetState(EState state);

	void BotMoveAndModeChange();


	UFUNCTION(Server,Reliable)
	void SRPC_SpawnBlock(TSubclassOf<class APSH_BlockActor> spawnActor);

	// ����ȭ�� �Բ� �����丵.
	// 1. ��� �õ�. ������ ���ٸ� 2������ �ִٸ� ������ 
	void Grab();

	// 2. ��� ���� Ray �õ�.
	void CastRay();

	// ���� �Ÿ�
	double rayPower = 1000.f;

	// ���콺 ��ġ�� �޾ƿ��� �Լ�
	FVector GetMouseDir();

	//3. �� ���� ���.
	UFUNCTION(Server,Reliable)
	void SRPC_Pickup(const FVector & startLoc, const FVector & endLoc);

	UFUNCTION(NetMulticast, Reliable)
	void MRPC_PickupAnim(class APSH_BlockActor* target);

	// Beak Effect
	UPROPERTY(EditDefaultsOnly,Category = Niagara)
	class UNiagaraSystem * pickEffect;

	UFUNCTION(Server , Reliable)
	void SRPC_PickEffect(FVector endLoc);

	UFUNCTION(NetMulticast , Reliable)
	void MRPC_PickEffect(FVector endLoc);
	
	//4. ���� �̵� -> Tick
	float PositionThreshold = 10.0f;
	FVector previousLocation;

	//5. ī�޶�� ���콺�� ��ġ�� �̿��� Ʈ���̽� �Ÿ� ��� ���͸� �˻���. << �ϴ���
	void PreTraceCheck( FVector & StartLoc,   FVector & EndLoc);

	// 6. ���콺 ��ġ�� ���� ���� ���͸� �����̰� �ٸ� ��ϰ� ��������� ����Ʈ�� �°� ������ ��ȯ
	UFUNCTION(NetMulticast,Unreliable)
	void MRPC_HandleBlock(FVector newLoc, FRotator newRot);
	UFUNCTION(Server,Unreliable)
	void SRPC_HandleBlock(FHitResult hitinfo, bool hit, FVector endLoc);

	// �� Rotation
	FRotator rotationOffset;

	int32 snapPointIndexLength;

	void ClosestPoint(TArray<FVector> pointArray, FVector testLocation, FTransform hitActorTransfrom,
		FVector& closestPt, float& dist, int32& closetPointIndex);

	float snapDistance = 10;

	int32 snapPointIndex = 0;

	//World Helper - ������ ������ �� ���� ���⿡ ���� ȸ�� ������ ���
	FRotator WorldHelperRotationOffset();

	// 7. �� ���̱�
	void PlaceBlock(FHitResult hitInfo, bool hit);

	UFUNCTION(Server,Reliable)
	void SRPC_PlaceBlock(FHitResult hitInfo, bool hit);

	void DropBlcok();
	UFUNCTION(NetMulticast,reliable)
	void MRPC_DropBlcok();

	TArray<class AActor*> actorsToIgnore;

	// �� �̵� �ִ�Ÿ� 
	UPROPERTY(EditAnywhere)
	float playerReach = 1000.f;

	float NormalizeAxis(float Angle);

	// �� ���� ��ȯ
	void HorizontalRotChange(const FInputActionValue& value);
	
	UFUNCTION(Server,Reliable)
	void SRPC_HorizontalRotChange(const FInputActionValue& value);

	UFUNCTION(NetMulticast,Reliable)
	void MRPC_HorizontalRotChange(const FInputActionValue& value);

	void VerticalRotChange(const FInputActionValue& value);

	UFUNCTION(Server,Reliable)
	void SRPC_VerticalRotChange(const FInputActionValue& value);

	UFUNCTION(NetMulticast,Reliable)
	void MRPC_VerticalRotChange(const FInputActionValue& value);

	// ������ȯ
	UPROPERTY()
	class APSH_BlockActor * GrabbedActor;

	void InterpToRotation(const float& DeltaTime);

	void MoisePosition(const float& DeltaTime);
	const float MaxDistance = 300.0f;  // ĳ���ͷκ����� �ִ� �Ÿ� ����
	const float MinDistance = 100.0f; // ĳ���ͷ��� �ּ� �Ÿ�

	// ���콺 �̵� ���� ���� ����
	const float MarginPercent = 0.05f; // 5% ����
	
	void SaveTest();
	int32 RowNum = 0;

	void DelegateTest();

	UFUNCTION(Server,Reliable)
	void SRPC_DelegateTest();
	UFUNCTION(Server,Reliable)
	void SRPC_Delegate();

	UFUNCTION()
	void Delegatebool(bool createMode);
};
