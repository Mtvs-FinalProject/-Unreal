// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/DedicatedServer/AutoRoomLevelInstance.h"
#include "Net/UnrealNetwork.h"
#include "LevelInstance/LevelInstanceSubsystem.h"
#include "PSH/PSH_DataTable/PSH_MechDataTable.h"
#include "PSH/PSH_Actor/PSH_BlockActor.h"
#include "JsonObjectConverter.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Serialization/JsonReader.h"
#include "Templates/SharedPointer.h"
#include "Json.h"
#include "Serialization/JsonSerializer.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerController.h"
#include "PSH/PSH_Player/PSH_Player.h"


AAutoRoomLevelInstance::AAutoRoomLevelInstance()
{
    bIsRoomAssigned = false;
    bIsLevelLoadedLocally = false;
    bReplicates = true;
    SetReplicatingMovement(true);
    bAlwaysRelevant = true;

    // �ʱ⿡�� WorldAsset�� �������� ����
    //SetWorldAsset(nullptr);
    
    // ���� ��� ���� (������Ʈ�� �°� ���� �ʿ�)
    LevelPath = TEXT("/Game/Alpha/NewFolder/runningmap_alpha.runningmap_alpha");
}


void AAutoRoomLevelInstance::ServerSpawnActorsFromJson_Implementation(const FString& JsonString)
{
    if (HasAuthority())
    {
        SpawnActorsFromJson(JsonString);
    }
}

void AAutoRoomLevelInstance::ServerAddPlayerToRoom_Implementation(APlayerController* PlayerController)
{
    if (HasAuthority() && !PlayersInRoom.Contains(PlayerController))
    {
        PlayersInRoom.Add(PlayerController);
        SpawnPlayerCharacter(PlayerController);
    }
}

void AAutoRoomLevelInstance::ServerRemovePlayerFromRoom_Implementation(APlayerController* PlayerController)
{
    if (HasAuthority())
    {
        PlayersInRoom.Remove(PlayerController);
        // If this was the last player, clean up the room
        if (PlayersInRoom.Num() == 0)
        {
            for (AActor* Actor : ManagedActors)
            {
                if (Actor)
                {
                    Actor->Destroy();
                }
            }
            ManagedActors.Empty();
            // Here you would also unload the level
        }
    }
}

void AAutoRoomLevelInstance::OnRep_ManagedActors()
{

}

void AAutoRoomLevelInstance::SpawnActorsFromJson(const FString& JsonString)
{
    TArray<TSharedPtr<FJsonValue>> JsonArray;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonArray))
    {
        for (const auto& JsonValue : JsonArray)
        {
            TSharedPtr<FJsonObject> ObjectData = JsonValue->AsObject();
            if (ObjectData.IsValid())
            {
                FPSH_ObjectData Data;
                if (FJsonObjectConverter::JsonObjectToUStruct(ObjectData.ToSharedRef(), &Data))
                {
                    FActorSpawnParameters SpawnParams;
                    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
                    SpawnParams.Owner = this;

                    FTransform SpawnTransform = Data.actorTransfrom;
                    SpawnTransform.SetLocation(GetActorLocation() + Data.actorTransfrom.GetLocation());

                    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(Data.actor, SpawnTransform, SpawnParams);
                    if (SpawnedActor)
                    {
                        ManagedActors.Add(SpawnedActor);
                        UE_LOG(LogTemp, Log, TEXT("Actor spawned: %s at location %s"),
                            *SpawnedActor->GetName(), *SpawnedActor->GetActorLocation().ToString());
                    }
                }
            }
        }
    }
}

void AAutoRoomLevelInstance::SpawnPlayerCharacter(APlayerController* PlayerController)
{
    // Find a PlayerStart in the loaded level
    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

    FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 100); // �⺻ ���� ��ġ
    FRotator SpawnRotation = FRotator::ZeroRotator;
    TSubclassOf<APSH_Player> PlayerClass = APSH_Player::StaticClass();
    if (PlayerStarts.Num() > 0)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        APlayerStart* Start = Cast<APlayerStart>(PlayerStarts[0]);
        if (Start)
        {
            SpawnLocation = Start->GetActorLocation();
            SpawnRotation = Start->GetActorRotation();
        }
        APSH_Player* NewPlayerCharacter = GetWorld()->SpawnActor<APSH_Player>(PlayerClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (NewPlayerCharacter)
        {
            PlayerController->Possess(NewPlayerCharacter);
            ManagedActors.Add(NewPlayerCharacter);
            UE_LOG(LogTemp, Log, TEXT("APSH_Player spawned and possessed by %s"), *PlayerController->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn APSH_Player."));
        }
    }
}

bool AAutoRoomLevelInstance::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
    const APlayerController* PC = Cast<APlayerController>(RealViewer);
    return PC && PlayersInRoom.Contains(const_cast<APlayerController*>(PC));
}

void AAutoRoomLevelInstance::OnRep_RuntimeWorldAsset()
{
    SetWorldAsset(RuntimeWorldAsset);
    UE_LOG(LogTemp, Log, TEXT("[%s] OnRep_WorldAsset: WorldAsset=%s, IsAssigned=%d, LoadedLocally=%d"),
        HasAuthority() ? TEXT("Server") : TEXT("Client"),
        *GetWorldAsset().ToSoftObjectPath().ToString(),
        bIsRoomAssigned,
        bIsLevelLoadedLocally);
    HandleWorldAssetChanged();
}

void AAutoRoomLevelInstance::HandleWorldAssetChanged()
{

    UE_LOG(LogTemp, Log, TEXT("HandleLevelLoadingState - IsAssigned: %d, LoadedLocally: %d"),
        bIsRoomAssigned, bIsLevelLoadedLocally);

    const bool bShouldBeLoaded = ShouldLoadLevelLocally();

    UE_LOG(LogTemp, Warning, TEXT("bShouldBeLoaded : %s"), bShouldBeLoaded ? TEXT("true") : TEXT("false"));

    UE_LOG(LogTemp, Warning, TEXT("bIsLevelLoadedLocally : %s"), bIsLevelLoadedLocally ? TEXT("true") : TEXT("false"));


    if (ULevelInstanceSubsystem* LevelInstanceSubsystem =
        GetWorld()->GetSubsystem<ULevelInstanceSubsystem>())
    {
        if (bShouldBeLoaded && !bIsLevelLoadedLocally)
        {
            UE_LOG(LogTemp, Log, TEXT("%s: Loading level instance (Authority: %d)"),
                *GetName(), HasAuthority());
            LevelInstanceSubsystem->RequestLoadLevelInstance(this, true);
            bIsLevelLoadedLocally = true;
        }
        else if (!bShouldBeLoaded && bIsLevelLoadedLocally)
        {
            UE_LOG(LogTemp, Log, TEXT("%s: Unloading level instance (Authority: %d)"),
                *GetName(), HasAuthority());
            LevelInstanceSubsystem->RequestUnloadLevelInstance(this);
            bIsLevelLoadedLocally = false;
        }
    }
}


bool AAutoRoomLevelInstance::ShouldLoadLevelLocally() const
{
    if (!bIsRoomAssigned || GetWorldAsset().IsNull())
    {
        UE_LOG(LogTemp, Log, TEXT("ShouldLoadLevelLocally - Early exit: IsAssigned=%d, HasWorldAsset=%d"),
            bIsRoomAssigned, !GetWorldAsset().IsNull());
        return false;
    }

    if (HasAuthority()) // ����
    {
        // ������ �濡 �÷��̾ �ִ� �� ���� ����
        bool bShouldLoad = ConnectedPlayers.Num() > 0;
        UE_LOG(LogTemp, Log, TEXT("ShouldLoadLevelLocally - Server: %d (Players: %d)"),
            bShouldLoad, ConnectedPlayers.Num());
        return bShouldLoad;
    }
    else // Ŭ���̾�Ʈ
    {
        APlayerController* LocalPlayer = GetWorld()->GetFirstPlayerController();
        bool bShouldLoad = IsPlayerInRoom(LocalPlayer);
        UE_LOG(LogTemp, Log, TEXT("ShouldLoadLevelLocally - Client: LocalPlayer=%s, InRoom=%d"),
            LocalPlayer ? *LocalPlayer->GetName() : TEXT("None"), bShouldLoad);
        return bShouldLoad;
    }
}

void AAutoRoomLevelInstance::BeginPlay()
{
    Super::BeginPlay();

    // �ʱ⿡�� ������ ��ε� ���·� ����
    if (ULevelInstanceSubsystem* LevelInstanceSubsystem =
        GetWorld()->GetSubsystem<ULevelInstanceSubsystem>())
    {
        SetRuntimeWorldAsset(nullptr);
        LevelInstanceSubsystem->RequestUnloadLevelInstance(this);
        bIsLevelLoadedLocally = false;
    }
}

void AAutoRoomLevelInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AAutoRoomLevelInstance, ManagedActors);
    DOREPLIFETIME(AAutoRoomLevelInstance, PlayersInRoom);
    DOREPLIFETIME(AAutoRoomLevelInstance, RoomName);
}

void AAutoRoomLevelInstance::ServerAssignAutoRoom_Implementation(const FString& NewRoomName)
{
    if (HasAuthority() && !bIsRoomAssigned)
    {
        UE_LOG(LogTemp, Log, TEXT("Server: Assigning auto room %s"), *NewRoomName);

        bIsRoomAssigned = true;
        SetCurrentRoomName(NewRoomName);
        SetRuntimeWorldAsset(TSoftObjectPtr<UWorld>(FSoftObjectPath(LevelPath)));
        // WorldAsset ���� - �̰��� �����Ǿ� Ŭ���̾�Ʈ�� �ڵ����� �ε��ϰ� ��
        //SetWorldAsset(TSoftObjectPtr<UWorld>(FSoftObjectPath(LevelPath)));


        OnRep_RoomState();
    }
}

void AAutoRoomLevelInstance::ServerUnassignAutoRoom_Implementation()
{
    if (HasAuthority() && bIsRoomAssigned)
    {
        UE_LOG(LogTemp, Log, TEXT("Server: Unassigning auto room %s"), *GetCurrentRoomName());

        bIsRoomAssigned = false;
        ConnectedPlayers.Empty();
        SetCurrentRoomName(TEXT(""));

        // WorldAsset ����
        SetRuntimeWorldAsset(nullptr);

        // WorldAsset ���� ó��
        //HandleWorldAssetChanged();

        OnRep_RoomState();
    }
}

void AAutoRoomLevelInstance::ServerJoinRoom_Implementation(APlayerController* JoiningPlayer)
{
    if (HasAuthority() && bIsRoomAssigned && JoiningPlayer)
    {
        if (!ConnectedPlayers.Contains(JoiningPlayer))
        {
            ConnectedPlayers.Add(JoiningPlayer);
            UE_LOG(LogTemp, Log, TEXT("Player %s joined room"), *JoiningPlayer->GetName());
            // ConnectedPlayers�� �����Ǿ� �� Ŭ���̾�Ʈ�� HandleLevelLoadingState�� ȣ���

            // �������� ���� �ε� ���� ����
            HandleWorldAssetChanged();
        }
    }
}


void AAutoRoomLevelInstance::ServerLeaveRoom_Implementation(APlayerController* LeavingPlayer)
{

    if (HasAuthority() && bIsRoomAssigned && LeavingPlayer)
    {
        // �ش� �÷��̾ ����
        if (ConnectedPlayers.Remove(LeavingPlayer))
        {
            UE_LOG(LogTemp, Log, TEXT("Player %s left room"), *LeavingPlayer->GetName());

            // ������ ������ �ٸ� �÷��̾ ������ ����
            if (ConnectedPlayers.Num() == 0)
            {
                // ������ �÷��̾ ������ ���� ���� ����
                //UE_LOG(LogTemp, Warning, TEXT("%s"))
                ServerUnassignAutoRoom();
            }
            else
            {
                // ���� �÷��̾��� Ŭ���̾�Ʈ���Ը� ���� ��ε� ��û
                ClientOnLeaveRoom(LeavingPlayer);
            }
        }
    }
}


void AAutoRoomLevelInstance::ClientOnLeaveRoom_Implementation(APlayerController* LeavingPlayer)
{
    // �ڽ��� ������ �÷��̾��� ��츸 ���� ��ε�
    if (!HasAuthority()) {
        if (LeavingPlayer && LeavingPlayer->IsLocalController())
        {
            if (ULevelInstanceSubsystem* LevelInstanceSubsystem =
                GetWorld()->GetSubsystem<ULevelInstanceSubsystem>())
            {
                UE_LOG(LogTemp, Log, TEXT("Client %s: Unloading level after leaving"),
                    *LeavingPlayer->GetName());
                LevelInstanceSubsystem->RequestUnloadLevelInstance(this);
                bIsLevelLoadedLocally = false;
            }
        }
    }

}

bool AAutoRoomLevelInstance::SetRuntimeWorldAsset(TSoftObjectPtr<UWorld> InWorldAsset)
{
    if (HasAuthority())
    {
        RuntimeWorldAsset = InWorldAsset;

        SetWorldAsset(RuntimeWorldAsset);

        HandleWorldAssetChanged();
        return true;
    }
    return false;
}

void AAutoRoomLevelInstance::OnRep_ConnectedPlayers()
{
    UE_LOG(LogTemp, Log, TEXT("OnRep_ConnectedPlayers - Player count: %d"), ConnectedPlayers.Num());
    // ConnectedPlayers�� ����� ������ ���� �ε� ���� üũ
    HandleWorldAssetChanged();
}

bool AAutoRoomLevelInstance::IsPlayerInRoom(APlayerController* PlayerController) const
{
    return ConnectedPlayers.Contains(PlayerController);
}

void AAutoRoomLevelInstance::OnRep_RoomState()
{
    UE_LOG(LogTemp, Log, TEXT("%s - Room state changed: IsAssigned=%d, RoomName=%s, HasWorldAsset=%d"),
        *GetName(),
        bIsRoomAssigned,
        *CurrentRoomName,
        !GetWorldAsset().IsNull());
}
