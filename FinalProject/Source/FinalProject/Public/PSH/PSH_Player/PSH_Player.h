// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputActionValue.h"
#include "../PSH_DataTable/PSH_MechDataTable.h"
#include "PSH_Player.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FLocationPoint // PreTraceChek ���� ����ϴ� ����, ���� ��ġ ����ü
{
	GENERATED_USTRUCT_BODY()
public:
	
	UPROPERTY(EditDefaultsOnly, Category = "LocationPoint")
	FVector startLocation;
	UPROPERTY(EditDefaultsOnly, Category = "LocationPoint")
	FVector endLocation;
};

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

	//Widgets 
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UUserWidget> mouseWidgetFac;
	class UPSH_MouseWidget* mouseWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UUserWidget> botWidgetFac;
	class UPSH_GarbageBotWidget* botWidget;

	//PC
	class APSH_PlayerController* pc;

	// �̵�
	FVector Dir;
	void Move(const FInputActionValue& value);

	void Look(const FInputActionValue& value);

	// �÷��̾� �ൿ
	void PlayerJump();

	// �� Rotation
	FRotator rotationOffset;

	int32 snapPointIndexLength;

	void ClosestPoint(TArray<FVector> pointArray, FVector testLocation, FTransform hitActorTransfrom,
		FVector& closestPt, float& dist, int32& closetPointIndex);

	float snapDistance = 10;

	int32 snapPointIndex = 0;

	//World Helper - ������ ������ �� ���� ���⿡ ���� ȸ�� ������ ���
	FRotator WorldHelperRotationOffset(); 

	void PlaceBlock(FHitResult hitInfo, bool hit);

	UFUNCTION(Server,Reliable)
	void SRPC_PlaceBlock(FHitResult hitInfo, bool hit);

	void DropBlcok();
	UFUNCTION(NetMulticast,Unreliable)
	void MRPC_DropBlcok();
	UFUNCTION(Server,Unreliable)
	void SRPC_DropBlcok();

	UFUNCTION(NetMulticast,Unreliable)
	void MRPC_HandleBlock(FVector newLoc, FRotator newRot);
	UFUNCTION(Server,Unreliable)
	void SRPC_HandleBlock(FHitResult hitinfo, bool hit ,FVector rayEndLocation);
	void HandleBlock(FHitResult hitinfo, bool hit ,FVector rayEndLocation);
	
	UPROPERTY(EditAnywhere)
	class UDataTable * dataTable;

	

	void SaveTest();

	int32 rowNam = 4;

	bool test = false;

	void LoadTest();

	// ����â? â ���� 
	void ShowInterface();

	void HorizontalRotChange(const FInputActionValue& value);

	void VerticalRotChange(const FInputActionValue& value);


	void OnArtKey(); // art Ȯ��
	
	bool bArtKey = false;

	// �κ� ����
	class APSH_GarbageBot * bot;

	void BotMoveAndModeChange();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FVector ReplicatedLocation;

	FRotator ReplicatedRotation;

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

	// Beak Effect
	UPROPERTY(EditDefaultsOnly,Category = Niagara)
	class UNiagaraSystem * pickEffect;

	UFUNCTION(NetMulticast , Reliable)
	void NRPC_PickEffect();

	FVector EffectEndLoc;

	// Spawn

	UPROPERTY(EditDefaultsOnly , Category = "Bot")
	TSubclassOf<class APSH_SpawnBot> spawnBotFac;

	UPROPERTY()
	class APSH_SpawnBot * spawnBot;

	bool bSpawn = true;

	UFUNCTION(Server,Reliable)
	void SRPC_SpawnBlock(TSubclassOf<class APSH_BlockActor> spawnActor);

	// ����ȭ�� �Բ� �����丵.
	// 1. ��� �õ�. ������ ���ٸ� 2������ �ִٸ� ������ 
	void Grab();

	// 2. ��� ���� Ray �õ�.
	void CastRay();

	// ���� �Ÿ�
	double rayPower = 1000.f;

	//3. �� ���� ���.
	UFUNCTION(Server,Reliable)
	void SRPC_Pickup(const FVector & startLoc, const FVector & endLoc);

	UFUNCTION(NetMulticast, Reliable)
	void MRPC_PickupAnim(class APSH_BlockActor* target);
	
	//4. ���� �̵� -> Tick

	//5. ī�޶�� ���콺�� ��ġ�� �̿��� Ʈ���̽� �Ÿ� ��� ���͸� �˻���. << �ϴ���
	void PreTraceCheck( FVector & StartLoc,   FVector & EndLoc);

	TArray<class AActor*> actorsToIgnore;

	// �� �̵� �ִ�Ÿ� 
	UPROPERTY(EditAnywhere)
	float playerReach = 1000.f;

	float NormalizeAxis(float Angle);


	// ������ȯ
	UPROPERTY()
	class APSH_BlockActor * GrabbedActor;
};
