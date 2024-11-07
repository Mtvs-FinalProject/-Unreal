// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputActionValue.h"
#include "../PSH_DataTable/PSH_MechDataTable.h"
#include "PSH_Player.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FLocationPoint // PreTraceChek 에서 사용하는 시작, 종료 위치 구조체
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

	float timeSinceLastSync = 0.0f;
	const float syncInterval = 0.1f;

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

	//Widgets 
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UUserWidget> mouseWidgetFac;
	class UPSH_MouseWidget* mouseWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<class UUserWidget> botWidgetFac;
	class UPSH_GarbageBotWidget* botWidget;

	//PC
	class APSH_PlayerController* pc;

	// 이동
	FVector Dir;
	void Move(const FInputActionValue& value);

	void Look(const FInputActionValue& value);

	// 플레이어 행동
	void PlayerJump();

	// 블럭 Rotation
	FRotator rotationOffset;

	int32 snapPointIndexLength;

	void ClosestPoint(TArray<FVector> pointArray, FVector testLocation, FTransform hitActorTransfrom,
		FVector& closestPt, float& dist, int32& closetPointIndex);

	float snapDistance = 10;

	int32 snapPointIndex = 0; 

	UFUNCTION(Server,Unreliable)
	void SRPC_DropBlcok();
	
	UPROPERTY(EditAnywhere)
	class UDataTable * dataTable;

	int32 rowNam = 4;

	bool test = false;

	void LoadTest();

	// 설정창? 창 열기 
	void ShowInterface();

	void HorizontalRotChange(const FInputActionValue& value);
	
	UFUNCTION(Server,Reliable)
	void SRPC_HorizontalRotChange(const FInputActionValue& value);

	UFUNCTION(NetMulticast,Reliable)
	void NRPC_HorizontalRotChange(const FInputActionValue& value);

	void VerticalRotChange(const FInputActionValue& value);

	UFUNCTION(Server,Reliable)
	void SRPC_VerticalRotChange(const FInputActionValue& value);

	UFUNCTION(NetMulticast,Reliable)
	void NRPC_VerticalRotChange(const FInputActionValue& value);

	void OnArtKey(); // art 확인
	
	bool bArtKey = false;

	// 로봇 관련
	class APSH_GarbageBot * garbagebot;

	void BotMoveAndModeChange();

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

	// Spawn

	UPROPERTY(EditDefaultsOnly , Category = "Bot")
	TSubclassOf<class APSH_SpawnBot> spawnBotFac;

	UPROPERTY()
	class APSH_SpawnBot * spawnBot;

	UFUNCTION(Client, Reliable)
	void CRPC_SetBot(class APSH_SpawnBot* spawn, class APSH_GarbageBot * garbage);
	UFUNCTION(NetMulticast, Reliable)
	void NRPC_SetBot(class APSH_SpawnBot* spawn, class APSH_GarbageBot * garbage);

	bool bSpawn = true;

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
	void NRPC_PickEffect(FVector endLoc);
	
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


	// 방향전환
	UPROPERTY()
	class APSH_BlockActor * GrabbedActor;
};
