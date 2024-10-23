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

	//PC
	class APSH_PlayerController* pc;

	// �̵�
	FVector Dir;
	void Move(const FInputActionValue& value);

	void Look(const FInputActionValue& value);

	// �÷��̾� �ൿ
	void PlayerJump();

	void Grab();

	// ���콺 ��ġ�� Ray
	void PreTraceCheck(FVector & StartLoc, FVector & EndLoc);

	TArray<class AActor*> actorsToIgnore;

	// �� �̵� �ִ�Ÿ�
	UPROPERTY(EditAnywhere)
	float playerReach = 1000.f;

	bool doonce;
	// Ray �߻�
	FHitResult CastRay();

	// ���� �Ÿ�
	double rayPower = 1000.f;

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

	void DropBlcok();

	void HandleBlock(FHitResult hitinfo, bool hit ,FVector rayEndLocation);
	
	UPROPERTY(EditAnywhere)
	class UDataTable * dataTable;
	void SpawnBlock();

	void RPCSpawnBlock(TSubclassOf<class APSH_BlockActor> & spawnActor);

	void SaveTest();

	int32 rowNam = 4;

	bool test = false;

	void LoadTest();

	// ����â? â ���� 
	void ShowInterface();

	void RightRotChange();
	void LeftRotChange();
	void SanpChange();
	void SanpRemove();
};
