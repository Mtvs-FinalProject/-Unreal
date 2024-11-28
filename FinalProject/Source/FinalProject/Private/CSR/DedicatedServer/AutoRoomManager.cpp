// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/DedicatedServer/AutoRoomManager.h"
#include "Net/UnrealNetwork.h"
#include "CSR/DedicatedServer/AutoRoomLevelInstance.h"
#include "CSR/DedicatedServer/AutoGameState.h"

// Sets default values
AAutoRoomManager::AAutoRoomManager()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    // 기본 설정
    MaxRooms = 10;
    SpaceBetweenRooms = 100000.0f;
}

void AAutoRoomManager::CreateAutoRoom(const FString& RoomName, APlayerController* RequestingPlayer)
{
    if (!ValidateRoomCreation(RoomName, RequestingPlayer))
    {
        return;
    }
    AAutoRoomLevelInstance* AvailableRoom = FindAvailableAutoRoom();
    if (AvailableRoom)
    {
        UE_LOG(LogTemp, Log, TEXT("Creating room %s for player %s"),
            *RoomName, *RequestingPlayer->GetName());
        AvailableRoom->RoomMode = ERoomMode::Create;
        AvailableRoom->ServerAssignAutoRoom(RoomName, TEXT(""));
        AvailableRoom->ServerJoinRoom(RequestingPlayer);
        UE_LOG(LogTemp, Warning, TEXT("csr %s"), *RoomName);
        if (AAutoGameState* GS = GetWorld()->GetGameState<AAutoGameState>())
        {
            UE_LOG(LogTemp, Warning, TEXT("csr hh %s"), *RoomName);

            GS->UpdateRoomList();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No available rooms for creation"));
    }
}


void AAutoRoomManager::JoinAutoRoom(const FString& RoomName, APlayerController* RequestingPlayer)
{
    if (!HasAuthority() || !RequestingPlayer)
    {
        UE_LOG(LogTemp, Warning, TEXT("JoinAutoRoom: Invalid state or player"));
        return;
    }

    AAutoRoomLevelInstance* Room = FindAutoRoomByName(RoomName);
    if (Room)
    {
        UE_LOG(LogTemp, Log, TEXT("Player %s joining room %s"),
            *RequestingPlayer->GetName(), *RoomName);
        Room->ServerJoinRoom(RequestingPlayer);
        // 여기서 추가적인 입장 로직 구현 가능
        if (AAutoGameState* GS = GetWorld()->GetGameState<AAutoGameState>())
        {
            GS->UpdateRoomList();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Room %s not found"), *RoomName);
    }
}


void AAutoRoomManager::LeaveAutoRoom(const FString& RoomName, APlayerController* RequestingPlayer)
{
	if (!HasAuthority() || !RequestingPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("LeaveAutoRoom: Invalid state or player"));
		return;
	}

    AAutoRoomLevelInstance* Room = FindAutoRoomByName(RoomName);
    if (Room)
    {
        UE_LOG(LogTemp, Log, TEXT("Player %s leaving room %s"),
            *RequestingPlayer->GetName(), *RoomName);
        // 마지막 플레이어가 나가면 방을 해제하는 로직 추가 가능
        //Room->ServerUnassignAutoRoom();
        Room->ServerLeaveRoom(RequestingPlayer);
        if (AAutoGameState* GS = GetWorld()->GetGameState<AAutoGameState>())
        {
            GS->UpdateRoomList();
        }
    }
}


void AAutoRoomManager::CreateAutoRoomWithData(const FString& RoomName, const FString& SelectedMap, const FString& JsonData, APlayerController* RequestingPlayer)
{
    if (!ValidateRoomCreation(RoomName, RequestingPlayer))
    {
        return;
    }

    AAutoRoomLevelInstance* AvailableRoom = FindAvailableAutoRoom();
    if (AvailableRoom)
    {
        UE_LOG(LogTemp, Warning, TEXT("csr %s"), *SelectedMap);
        UE_LOG(LogTemp, Log, TEXT("Creating room %s with data for player %s"),
            *RoomName, *RequestingPlayer->GetName());
        AvailableRoom->RoomMode = ERoomMode::Play;
        AvailableRoom->SetSelectedMap(SelectedMap);
        AvailableRoom->ServerAssignAutoRoom(RoomName, JsonData);
        AvailableRoom->ServerJoinRoom(RequestingPlayer);
        if (AAutoGameState* GS = GetWorld()->GetGameState<AAutoGameState>())
        {
            GS->UpdateRoomList();
        }
    }
}

AAutoRoomLevelInstance* AAutoRoomManager::FindAvailableAutoRoom() const
{
    for (AAutoRoomLevelInstance* Room : AutoRoomPool)
    {
        if (Room && !Room->bIsRoomAssigned)
        {
            return Room;
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("AAutoRoomManager::FindAvailableAutoRoom Warring"));
    return nullptr;
}


AAutoRoomLevelInstance* AAutoRoomManager::FindAutoRoomByName(const FString& RoomName) const
{
    for (AAutoRoomLevelInstance* Room : AutoRoomPool)
    {
        if (Room && Room->bIsRoomAssigned && Room->GetCurrentRoomName() == RoomName)
        {
            return Room;
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("AAutoRoomManager::FindAutoRoomByName Warring"));
    return nullptr;
}

// Called when the game starts or when spawned
void AAutoRoomManager::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        // GameState에 등록
        if (AAutoGameState* GameState = GetWorld()->GetGameState<AAutoGameState>())
        {
            UE_LOG(LogTemp, Log, TEXT("Registering AutoRoomManager to GameState"));
            GameState->AutoRoomManager = this;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to find GameState"));
            return;
        }

        for (int32 i = 0; i < MaxRooms; ++i)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;

            const int32 Row = i / 3;
            const int32 Col = i % 3;

            FVector SpawnLocation(
                Col * SpaceBetweenRooms,
                Row * SpaceBetweenRooms,
                0.0f
            );

            AAutoRoomLevelInstance* NewRoom = GetWorld()->SpawnActor<AAutoRoomLevelInstance>(
                AAutoRoomLevelInstance::StaticClass(),
                SpawnLocation,
                FRotator::ZeroRotator,
                SpawnParams
            );

            if (NewRoom)
            {
                AutoRoomPool.Add(NewRoom);
                UE_LOG(LogTemp, Log, TEXT("Created AutoRoomLevelInstance %d at location %s"),
                    i, *SpawnLocation.ToString());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to create AutoRoomLevelInstance %d"), i);
            }
        }
    }

}

void AAutoRoomManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    // 모든 방 정리
    if (HasAuthority())
    {
        for (AAutoRoomLevelInstance* Room : AutoRoomPool)
        {
            if (Room && Room->bIsRoomAssigned)
            {
                Room->ServerUnassignAutoRoom();
            }
        }
    }
}

void AAutoRoomManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AAutoRoomManager, AutoRoomPool);
}

bool AAutoRoomManager::ValidateRoomCreation(const FString& RoomName, APlayerController* RequestingPlayer) const
{
    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateAutoRoom: No Authority"));
        return false;
    }

    if (!RequestingPlayer)
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateAutoRoom: Invalid player"));
        return false;
    }

    if (FindAutoRoomByName(RoomName))
    {
        UE_LOG(LogTemp, Warning, TEXT("Room with name %s already exists"), *RoomName);
        return false;
    }

    return true;
}
