// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/PSH_Actor/PSH_GarbageBot.h"
#include "PSH/PSH_Player/PSH_Player.h"
#include "PSH/PSH_Actor/PSH_BlockActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APSH_GarbageBot::APSH_GarbageBot()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    boxCol = CreateDefaultSubobject<UBoxComponent>(TEXT("boxCol"));
    SetRootComponent(boxCol);

	compMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	compMesh->SetupAttachment(RootComponent);
    compMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

// Called when the game starts or when spawned
void APSH_GarbageBot::BeginPlay()
{
	Super::BeginPlay();

    boxCol->OnComponentBeginOverlap.AddDynamic(this,&APSH_GarbageBot::BeginOverlap);

	player = Cast<APSH_Player>(GetWorld()->GetFirstPlayerController()->GetCharacter());
    
    if (player)
    {
        SetOwner(player);
        InitializeMovePoint();
    }
}

// Called every frame
void APSH_GarbageBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // �����
//     const FString myState = UEnum::GetValueAsString(state);
//  	DrawDebugString(GetWorld(), GetActorLocation(), myState, nullptr, FColor::Red, 0, true);

    if (player)
    {
        // �÷��̾��� ��ġ
        FVector PlayerLocation = player->GetActorLocation();
        FVector CurrentLocation = GetActorLocation();
        FRotator CurrentRotation = GetActorRotation();
        FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, PlayerLocation);

        // �ε巯�� ȸ��
        FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, NewRotation, DeltaTime, 5.0f); // 5.0f�� ȸ�� �ӵ�

        SetActorRotation(SmoothedRotation);
    }
	switch (state)
	{
	case EState::IDLE: IdleState();
		break;
	case EState::MOVE: MoveState(DeltaTime);
		break;
    case EState::Destroy: DestroyState();
        break;
	}
}

void APSH_GarbageBot::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	APSH_BlockActor * block = Cast<APSH_BlockActor>(OtherActor);

	if (block && !block->pickedUp && bDestroy)
	{
		block->AllDestroy();
	}
}

void APSH_GarbageBot::IdleState()
{
    if(player == nullptr) return;
   float distance = FVector::Dist(GetActorLocation() , player->GetActorLocation());
}

void APSH_GarbageBot::MoveState(const float& deltaTime)
{
    if (player == nullptr) return;

    // ���� ��ġ���� ��ǥ ��ġ������ ���Ϳ� �Ÿ�
    FVector TargetDirection = (Dir - GetActorLocation()).GetSafeNormal();
    float DistanceToTarget = FVector::Dist(GetActorLocation(), Dir);

    // ��ǥ ��ġ�� ������ ��� �̵� ����
    if (DistanceToTarget <= StopDistanceThreshold) // ��: StopDistanceThreshold = 5.f
    {
        return; // �̵� ����
    }

    // ��ǥ ��ġ���� �̵�
    FVector NewLoc = GetActorLocation() + TargetDirection * MoveSpeed * deltaTime;
    SetActorLocation(NewLoc);
   
}

void APSH_GarbageBot::DestroyState()
{

    FHitResult outHit;
    FVector start = GetActorLocation();
    FVector end = start + (GetActorUpVector() * 500);
    ECollisionChannel Visibility = ECC_Visibility;

    // ĸ���� �������� ���̸� ���� (��� ũ�⿡ �°� ����)
    float capsuleRadius = GetActorScale3D().X * 50.0f;
    float capsuleHalfHeight = GetActorScale3D().Z * 100.0f;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bhit = GetWorld()->SweepSingleByChannel(
        outHit,
        start,
        end,
        FQuat::Identity,
        Visibility,
        FCollisionShape::MakeCapsule(capsuleRadius, capsuleHalfHeight),
        Params
    );

    APSH_BlockActor* block = Cast<APSH_BlockActor>(outHit.GetActor());
    if (bhit && block)
    {
        // ��Ʈ�� ��ġ���� ����׿� ���� ǥ��
        UE_LOG(LogTemp, Warning, TEXT("%s"), *outHit.GetActor()->GetName());

        if (sumScale >= 1.85f)
        {
            return;
        }
        bDestroy = true;
        sumScale += GetWorld()->DeltaTimeSeconds;
        SetActorScale3D(scale * sumScale);
    }
    else
    {
        if (sumScale <= 0.25f)
        {
            return;
        }
        bDestroy = false;
        sumScale -= GetWorld()->DeltaTimeSeconds;
        SetActorScale3D(scale * sumScale);
    }
}

void APSH_GarbageBot::SetState(EState nextState)
{
    state = nextState;
}


void APSH_GarbageBot::MoveToLocation(const class APSH_Player& owner, const FVector& TargetLocation)
{
    Dir = TargetLocation;

    Dir.Z = GetActorLocation().Z;

    // �̵� ��� ���
//     NewLoc = GetActorLocation() + Dir * MoveSpeed * GetWorld()->DeltaTimeSeconds;

}

void APSH_GarbageBot::InitializeMovePoint()
{
    if (player)
    {
        Dir = player->GetActorLocation() + GetActorForwardVector() * 250;
    }
}
TArray<FNavigationNode*> APSH_GarbageBot::FindPath(FNavigationNode* StartNode, FNavigationNode* TargetNode)
{
    if (!StartNode || !TargetNode) 
    {
        UE_LOG(LogTemp,Error,TEXT("Error")); 
        return TArray<FNavigationNode*>(); // �߸��� �Է� ó��
    }

    TArray<FNavigationNode*> OpenSet;
    TArray<FNavigationNode*> ClosedSet;
    OpenSet.Add(StartNode);

    StartNode->GCost = 0;
    StartNode->HCost = CalculateHeuristic(StartNode, TargetNode);
    StartNode->FCost = StartNode->CalculateFCost();
    StartNode->CameFrom = nullptr;

    while (OpenSet.Num() > 0)
    {
        FNavigationNode* CurrentNode = GetNodeWithLowestFCost(OpenSet);
        if (CurrentNode == TargetNode)
        {
            // ��θ� ã���� ��ȯ
            TArray<FNavigationNode*> Path;
            while (CurrentNode != nullptr)
            {
                Path.Insert(CurrentNode, 0);
                CurrentNode = CurrentNode->CameFrom;
            }
            return Path;
        }

        OpenSet.Remove(CurrentNode);
        ClosedSet.Add(CurrentNode);

        for (FNavigationNode* Neighbor : CurrentNode->Neighbors)
        {
            if (ClosedSet.Contains(Neighbor))
                continue;

            float TentativeGCost = CurrentNode->GCost + FVector::Dist(CurrentNode->NodeLocation, Neighbor->NodeLocation);

            if (TentativeGCost < Neighbor->GCost || !OpenSet.Contains(Neighbor))
            {
                Neighbor->GCost = TentativeGCost;
                Neighbor->HCost = CalculateHeuristic(Neighbor, TargetNode);
                Neighbor->FCost = Neighbor->CalculateFCost();
                Neighbor->CameFrom = CurrentNode;

                if (!OpenSet.Contains(Neighbor))
                {
                    OpenSet.Add(Neighbor);
                }
            }
        }
    }

    return TArray<FNavigationNode*>();  // ��θ� ã�� ������ �� �� �迭 ��ȯ
}
FNavigationNode* APSH_GarbageBot::GetClosestNodeToActor(AActor* Actor)
{
   
    if (Actor == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Actor Null"));
        return nullptr;
    }
  

    FNavigationNode* ClosestNode = nullptr;
    float ClosestDistance = FLT_MAX;

    for (FNavigationNode& Node : NavigationNodes)
    {
        float Distance = FVector::Dist(Node.NodeLocation, player->GetActorLocation());
        if (Distance < ClosestDistance)
        {
            ClosestDistance = Distance;
            ClosestNode = &Node;
        }
    }

    return ClosestNode;
}

void APSH_GarbageBot::InitializeNavigationNodes()
{
    FNavigationNode Node1;
    Node1.NodeLocation = FVector(0, 0, 0);
    NavigationNodes.Add(Node1);

    FNavigationNode Node2;
    Node2.NodeLocation = FVector(100, 0, 0);
    NavigationNodes.Add(Node2);
}

float APSH_GarbageBot::CalculateHeuristic(FNavigationNode* FromNode, FNavigationNode* ToNode)
{
    return FVector::Dist(FromNode->NodeLocation, ToNode->NodeLocation);
}
FNavigationNode* APSH_GarbageBot::GetNodeWithLowestFCost(TArray<FNavigationNode*>& OpenSet)
{
    FNavigationNode* LowestFCostNode = OpenSet[0];
    for (FNavigationNode* Node : OpenSet)
    {
        if (Node->FCost < LowestFCostNode->FCost)
        {
            LowestFCostNode = Node;
        }
    }
    return LowestFCostNode;
}