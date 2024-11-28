// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../PSH_DataTable/PSH_MechDataTable.h"
#include "PSH_BlockActor.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FComponentCreateBoolDelegate, bool);

UENUM(BlueprintType)
enum class EBlockDataType : uint8
{
	NOMAL UMETA(DisplayName = "Nomal"),
	MOVEANDFLY UMETA(DisplayName = "MoveAndFly"),
	ROTATE UMETA(DisplayName = "Rotate"),
};

UCLASS()
class FINALPROJECT_API APSH_BlockActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APSH_BlockActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//델리게이트 등록
	FComponentCreateBoolDelegate componentCreateBoolDelegate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UStaticMeshComponent * meshComp;

	/// 블럭 자식 오브젝트 배열
	UPROPERTY(EditAnywhere)
	TArray<class AActor*> childsActors;

	// 부착 위치
	UPROPERTY(EditAnywhere)
	TArray<FVector> snapPoints;

	UPROPERTY(EditAnywhere)
	TArray<FRotator> snapDirections; // 블럭 지도.?

	UPROPERTY(EditAnywhere)
	TArray<int32> snapPritority; // 블럭 우선사항

	int32 connectionsize =0;

	bool validPlacement;

	UPROPERTY(BlueprintReadOnly)
	bool pickedUp;


	UFUNCTION(NetMulticast,Reliable)
	void MRPC_PickUp(class UPhysicsHandleComponent* handle);
	void PickUp(class UPhysicsHandleComponent* handle);

	void Remove();
	
	UFUNCTION(NetMulticast,Reliable)
	void MRPC_Remove();

	UFUNCTION(Server,Reliable)
	void SRPC_Remove();

	bool bGrab = false;
	
	UPROPERTY(EditAnywhere)
	class APSH_BlockActor * parent;

	TArray<FVector> GetSnapPoints();

	TArray<FRotator> GetSnapDirections();

	TArray<int32> GetsnapPritority();

	void AddSnapPoint(FVector location, FRotator rotation, int32 priority);

	bool OvelapChek();

	void SnapApplyPriority();

	void Drop(class UPhysicsHandleComponent * physicshandle);

	UFUNCTION(NetMulticast,Reliable)
	void MRPC_Drop(class UPhysicsHandleComponent * physicshandle);


	void Place(class APSH_BlockActor* attachActor, FTransform worldTransform);
	
	UFUNCTION(NetMulticast,Reliable)
	void MRPC_Place(class APSH_BlockActor* attachActor, FTransform worldTransform);

	void AddChild(class APSH_BlockActor* childActor);

	void ChildCollisionUpdate(ECollisionEnabled::Type NewType);

	void TransferChildren(TArray<AActor*> childActor);

	void RemoveChild(class APSH_BlockActor * actor);
	void RemoveChildren(TArray<AActor*> childActor);

	UFUNCTION()
	void OnComponentSleep(UPrimitiveComponent* SleepingComponent, FName BoneName);

	FPSH_ObjectData SaveBlock();
	FPSH_BlockData SaveChildBlock();

	FTransform placedWorldTransform;
	bool bIsPlaced = false;
	//FPSH_Childdats SaveBlock();
	
	//void LoadBlockHierarchy(class APSH_BlockActor* attachActor,const FPSH_BlockData& Childdats);
	void LoadBlockHierarchy(const FPSH_BlockData& data);

	void AllDestroy();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 특정 기능 컴포넌트 추가
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//class UMyFlyActorComponent* MyFlyActorComponent;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//class UMyMoveActorComponent* MyMoveActorComponent;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//class UMyRotateActorComponent* MyRotateActorComponent;

	// 플레이어 중복잡기 방지를 위해 오너 정해주기 ( Net에서 사용하는 GetSetOwner과 다르게 작동을 위해 직접 생성)
	void SetMaster(class APSH_Player* owner);

	APSH_Player * GetMaster();
	
	UFUNCTION(Server,Unreliable)
	void SRPC_BlockScale(float axis);

	UFUNCTION(NetMulticast,Unreliable)
	void MRPC_BlockScale(FVector scaleVec);

	float blockScale = 1;

	FVector scale = FVector(1.f);

	// 잡았을때 이펙트 생성
	UPROPERTY(EditDefaultsOnly)
	class UMaterial * outLineMat;

	void SetOutLine(bool chek);
	
	UFUNCTION(NetMulticast,Reliable)
	void MRPC_SetOutLine(bool chek);

	UPROPERTY(EditAnywhere)
	EBlockDataType blockDataType = EBlockDataType::NOMAL;

	UPROPERTY(EditDefaultsOnly)
	bool mapBlock = false;

// 	UFUNCTION(Server,Reliable)
// 	void SRPC_

	UFUNCTION()
	void StartBlockDelgate(bool createMode);

	UFUNCTION(NetMulticast,Reliable)
	void MRPC_StartBlockDelgate(bool createMode);

	FPSH_ObjectData locationData;

	void SaveBlockLocations();

	bool bisSavePoint = false;

	TArray<FPSH_FunctionBlockData> ComponentSaveData(EBlockDataType dataType);

	void ComponentLoadData(TArray<FPSH_FunctionBlockData> funcionBlockData);

	class UMyFlyActorComponent* flyComp;
	class UMyMoveActorComponent * moveComp;
	class UMyRotateActorComponent* rotationMovementComp;

	class UNiagaraSystem* spawnEffect;
	class UNiagaraSystem* PlaceEffect;

	UFUNCTION()
	void OnComponentHit( UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit );

	UFUNCTION(NetMulticast,Reliable)
	void MRPC_SpawnEffect(const FVector& impactPoint);

	bool bHit = true;

	UFUNCTION(Server,Reliable)
	void SRPC_SetSimulatePhysics(bool check);

	UFUNCTION(NetMulticast,Reliable)
	void MRPC_SetSimulatePhysics(bool check);

	UFUNCTION(NetMulticast,Reliable)
	void MRPC_LoadSetting();

private:
	class APSH_Player * master = nullptr;



// 성락 코드
#pragma region 
public:
// 
//     // 네트워크 관련성 함수 오버라이드
//     virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;

public:
// 
// 	// 레벨 인스턴스 소유권 설정
// 	void SetOwnedByRoomInstance(bool bInIsOwnedByRoomInstance)
// 	{
// 		bIsOwnedByRoomInstance = bInIsOwnedByRoomInstance;
// 	}
// 
// 	// 레벨 인스턴스 소유 여부 확인
// 	bool IsOwnedByRoomInstance() const { return bIsOwnedByRoomInstance; }
// 
// protected:
// 	virtual void PostInitializeComponents() override;
private:
	//UPROPERTY(Replicated)
 //   bool bIsOwnedByRoomInstance;
#pragma endregion

};
