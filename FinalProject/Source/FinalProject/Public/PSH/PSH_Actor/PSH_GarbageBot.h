// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PSH_GarbageBot.generated.h"

// ��带 ��Ÿ���� ����ü ����
struct FNavigationNode
{
	FVector NodeLocation;                // ��� ��ġ
	TArray<FNavigationNode*> Neighbors;  // �̿� ���
	float GCost;                         // ���� �������� ���� ���������� ���
	float HCost;                         // ��ǥ ���������� ���� ���
	float FCost;                         // GCost + HCost
	FNavigationNode* CameFrom;           // ���� ��� (��� ������)

	FNavigationNode()
		: GCost(FLT_MAX), HCost(0), FCost(FLT_MAX), CameFrom(nullptr) {}

	float CalculateFCost() const
	{
		return GCost + HCost;
	}

	void ResetCosts()
	{
		GCost = FLT_MAX;
		HCost = 0;
		FCost = FLT_MAX;
		CameFrom = nullptr;
	}
};


UENUM(BlueprintType)
enum class EState : uint8
{
	IDLE ,
	MOVE ,
	Destroy,
};

UCLASS()
class FINALPROJECT_API APSH_GarbageBot : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APSH_GarbageBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	class APSH_Player * player;

	UPROPERTY(EditAnywhere)
	class UBoxComponent * boxCol;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent * compMesh;

	float sumScale = 0.25;

	FVector scale = FVector(1.f);
	FVector minScale = FVector(0.25f);

	UPROPERTY(EditAnywhere)
	EState state = EState::IDLE;

	void IdleState();
	void MoveState(const float & deltaTime);
	void DestroyState();

	void SetState(EState nextState);

	

	float moveDist = 750;

	FVector Dir;

	float StopDistanceThreshold = 5.f;
	float MoveSpeed = 400;


	UFUNCTION()
	void BeginOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep
	, const FHitResult& SweepResult);
	// ��ã�� ���
public:
	TArray<FNavigationNode*> CurrentPath; // ���� ���
	int32 CurrentPathIndex; // ���� ��ǥ ��� �ε���

	// ��� ����Ʈ
	TArray<FNavigationNode> NavigationNodes;

	// A* �˰��� ����
	TArray<FNavigationNode*> FindPath(FNavigationNode* StartNode, FNavigationNode* TargetNode);

	FNavigationNode* GetClosestNodeToActor(AActor* Actor);

	void InitializeNavigationNodes();

	void MoveToLocation(const class APSH_Player& owner,const FVector& TargetLocation);

	void InitializeMovePoint();
	
private:

	bool bDestroy = false;
	float CalculateHeuristic(FNavigationNode* FromNode, FNavigationNode* ToNode);
	FNavigationNode* GetNodeWithLowestFCost(TArray<FNavigationNode*>& OpenSet);
};
