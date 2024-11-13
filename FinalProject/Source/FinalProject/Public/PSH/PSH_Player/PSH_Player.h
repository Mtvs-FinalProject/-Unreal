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
	class UPhysicsHandleComponent * handleComp; // 물리 컴포핸들 컴포넌트.

	UPROPERTY(EditDefaultsOnly)
	class USceneComponent * rotationHelper; // 회전 도우미

	UPROPERTY()
	class UCharacterMovementComponent* movementComp;
	
	//PC
	class APSH_PlayerController* pc;
	void InitPcUi();

	// 이동
	FVector Dir;
	void Move(const FInputActionValue& value);

	void Look(const FInputActionValue& value);

	// 플레이어 행동
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

	// 설정창? 창 열기 
	void ShowInterface();

	void OnArtKey(); // art 확인
	
	//스케일 조정
	void OnBlockScale(const FInputActionValue & value);

	bool bArtKey = false;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 메인 UI 연결
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> FirstSelect;

	// 현재 열려 있는 UI 위젯 인스턴스를 참조 (UI 열고 닫기 토글 상태 추적)
    UPROPERTY()
    UUserWidget* CurrentObjectWidget = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UMaterial * previewMat;

	// 카메라 조정
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

	// 서버화와 함께 리펙토링.
	// 1. 잡기 시도. 잡은게 없다면 2번으로 있다면 번으로 
	void Grab();

	// 2. 잡기 위한 Ray 시도.
	void CastRay();

	// 레이 거리
	double rayPower = 1000.f;

	// 마우스 위치를 받아오는 함수
	FVector GetMouseDir();

	//3. 블럭 실제 잡기.
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
	
	//4. 블럭의 이동 -> Tick
	float PositionThreshold = 10.0f;
	FVector previousLocation;

	//5. 카메라와 마우스의 위치를 이용해 트레이스 거리 계산 엑터만 검사함. << 하던중
	void PreTraceCheck( FVector & StartLoc,   FVector & EndLoc);

	// 6. 마우스 위치에 따라 잡은 엑터를 움직이고 다른 블록과 닿아있으면 포인트에 맞게 방향을 전환
	UFUNCTION(NetMulticast,Unreliable)
	void MRPC_HandleBlock(FVector newLoc, FRotator newRot);
	UFUNCTION(Server,Unreliable)
	void SRPC_HandleBlock(FHitResult hitinfo, bool hit, FVector endLoc);

	// 블럭 Rotation
	FRotator rotationOffset;

	int32 snapPointIndexLength;

	void ClosestPoint(TArray<FVector> pointArray, FVector testLocation, FTransform hitActorTransfrom,
		FVector& closestPt, float& dist, int32& closetPointIndex);

	float snapDistance = 10;

	int32 snapPointIndex = 0;

	//World Helper - 고정된 액터의 면 스냅 방향에 대한 회전 오프셋 계산
	FRotator WorldHelperRotationOffset();

	// 7. 블럭 붙이기
	void PlaceBlock(FHitResult hitInfo, bool hit);

	UFUNCTION(Server,Reliable)
	void SRPC_PlaceBlock(FHitResult hitInfo, bool hit);

	void DropBlcok();
	UFUNCTION(NetMulticast,reliable)
	void MRPC_DropBlcok();

	TArray<class AActor*> actorsToIgnore;

	// 블럭 이동 최대거리 
	UPROPERTY(EditAnywhere)
	float playerReach = 1000.f;

	float NormalizeAxis(float Angle);

	// 블럭 방향 전환
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

	// 방향전환
	UPROPERTY()
	class APSH_BlockActor * GrabbedActor;

	void InterpToRotation(const float& DeltaTime);

	void MoisePosition(const float& DeltaTime);
	const float MaxDistance = 300.0f;  // 캐릭터로부터의 최대 거리 제한
	const float MinDistance = 100.0f; // 캐릭터로의 최소 거리

	// 마우스 이동 제한 영역 설정
	const float MarginPercent = 0.05f; // 5% 여백
	
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
