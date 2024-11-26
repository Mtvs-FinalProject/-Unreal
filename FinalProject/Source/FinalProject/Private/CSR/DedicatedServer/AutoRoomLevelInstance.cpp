// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/DedicatedServer/AutoRoomLevelInstance.h"
#include "Net/UnrealNetwork.h"
#include "LevelInstance/LevelInstanceSubsystem.h"
#include "PSH/PSH_DataTable/PSH_MechDataTable.h"
#include "PSH/PSH_Actor/PSH_BlockActor.h"
#include "JsonObjectConverter.h"
#include "GameFramework/GameModeBase.h"
#include "PSH/PSH_GameMode/PSH_GameModeBase.h"
#include "PSH/PSH_Player/PSH_Player.h"


AAutoRoomLevelInstance::AAutoRoomLevelInstance()
{
    bIsRoomAssigned = false;
    bIsLevelLoadedLocally = false;
    bReplicates = true;
    SetReplicatingMovement(true);
    bAlwaysRelevant = true;

    // 초기에는 WorldAsset을 설정하지 않음
    //SetWorldAsset(nullptr);
    
    // 레벨 경로 설정 (프로젝트에 맞게 수정 필요)
    LevelPath = TEXT("/Game/PSH/PSH_Map/PSH_Level.PSH_Level");
}

void AAutoRoomLevelInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AAutoRoomLevelInstance, bIsRoomAssigned);
    DOREPLIFETIME(AAutoRoomLevelInstance, RuntimeWorldAsset);
    // ConnectedPlayers가 변경될 때마다 알림을 받도록 수정
    DOREPLIFETIME_CONDITION_NOTIFY(AAutoRoomLevelInstance, ConnectedPlayers, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME(AAutoRoomLevelInstance, CurrentRoomName);

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

    if (HasAuthority()) // 서버
    {
        // 서버는 방에 플레이어가 있는 한 레벨 유지
        bool bShouldLoad = ConnectedPlayers.Num() > 0;
        UE_LOG(LogTemp, Log, TEXT("ShouldLoadLevelLocally - Server: %d (Players: %d)"),
            bShouldLoad, ConnectedPlayers.Num());
        return bShouldLoad;
    }
    else // 클라이언트
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

    // 초기에는 레벨을 언로드 상태로 시작
    if (ULevelInstanceSubsystem* LevelInstanceSubsystem =
        GetWorld()->GetSubsystem<ULevelInstanceSubsystem>())
    {
        SetRuntimeWorldAsset(nullptr);
        LevelInstanceSubsystem->RequestUnloadLevelInstance(this);
        bIsLevelLoadedLocally = false;
    }
}

void AAutoRoomLevelInstance::ServerAssignAutoRoom_Implementation(const FString& NewRoomName, const FString& JsonData)
{
    if (HasAuthority() && !bIsRoomAssigned)
    {
        UE_LOG(LogTemp, Log, TEXT("Server: Assigning auto room %s"), *NewRoomName);

        PendingJsonData = JsonData;
        bIsRoomAssigned = true;
        SetCurrentRoomName(NewRoomName);
        SetRuntimeWorldAsset(TSoftObjectPtr<UWorld>(FSoftObjectPath(LevelPath)));
        // WorldAsset 설정 - 이것이 복제되어 클라이언트도 자동으로 로드하게 됨
        //SetWorldAsset(TSoftObjectPtr<UWorld>(FSoftObjectPath(LevelPath)));


        OnRep_RoomState();
    }
}

void AAutoRoomLevelInstance::ServerUnassignAutoRoom_Implementation()
{
    if (HasAuthority() && bIsRoomAssigned)
    {
        UE_LOG(LogTemp, Log, TEXT("Unassigning room %s, cleaning up %d actors"),
            *GetCurrentRoomName(), SpawnedActors.Num());
        
        CleanupSpawnedActors();
        // 방 상태 초기화
        bIsRoomAssigned = false;
        ConnectedPlayers.Empty();
        SetCurrentRoomName(TEXT(""));

        // WorldAsset 제거
        SetRuntimeWorldAsset(nullptr);

        // WorldAsset 변경 처리
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
            // ConnectedPlayers가 복제되어 각 클라이언트의 HandleLevelLoadingState가 호출됨

            // 서버에서 레벨 로드 상태 갱신
            HandleWorldAssetChanged();
            SpawnAndSetupCharacter(JoiningPlayer);
        }
    }
}


void AAutoRoomLevelInstance::ServerLeaveRoom_Implementation(APlayerController* LeavingPlayer)
{

    if (HasAuthority() && bIsRoomAssigned && LeavingPlayer)
    {
        if (ConnectedPlayers.Remove(LeavingPlayer) > 0)
        {
            ClientOnLeaveRoom(LeavingPlayer);

            if (ConnectedPlayers.Num() == 0)
            {
                ServerUnassignAutoRoom();
            }
            else
            {
                HandleWorldAssetChanged();
            }
        }
    }
}


void AAutoRoomLevelInstance::ClientOnLeaveRoom_Implementation(APlayerController* LeavingPlayer)
{
    // 자신이 나가는 플레이어인 경우만 레벨 언로드
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
    // ConnectedPlayers가 변경될 때마다 레벨 로드 상태 체크
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

// 레벨 스폰 관련
#pragma region 
void AAutoRoomLevelInstance::OnLevelInstanceLoaded()
{
    Super::OnLevelInstanceLoaded();

    if (!HasAuthority() || PendingJsonData.IsEmpty())
    {
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Level loaded for room %s, spawning actors..."), *this->GetCurrentRoomName());
    UE_LOG(LogTemp, Log, TEXT("Level loaded for room %s, spawning actors..."), *this->PendingJsonData);
    // JSON 데이터로 액터 스폰
    SpawnActorsFromJson();
}

void AAutoRoomLevelInstance::SpawnActorsFromJson()
{
    if (!HasAuthority() || PendingJsonData.IsEmpty())
    {
        return;
    }

    // 임시 데이터 테이블 생성 및 JSON 데이터 로드
    UDataTable* TempDataTable = NewObject<UDataTable>(this);
    TempDataTable->RowStruct = FPSH_ObjectData::StaticStruct();

    // JSON 문자열을 데이터 테이블로 변환
    TempDataTable->CreateTableFromJSONString(PendingJsonData);

    // 데이터 로드 및 액터 스폰
    TArray<FPSH_ObjectData*> DataArray;
    TempDataTable->GetAllRows<FPSH_ObjectData>(TEXT(""), DataArray);

    FVector SpawnOrigin = GetActorLocation();

    for (FPSH_ObjectData* Data : DataArray)
    {
        if (Data && Data->actor)
        {
            FTransform SpawnTransform = Data->actorTransfrom;
            SpawnTransform.SetLocation(SpawnTransform.GetLocation() + SpawnOrigin);
            FActorSpawnParameters SpawnParams;
            //SpawnParams.bDeferConstruction = true;  // 추가
            SpawnParams.Owner = this;
            SpawnParams.bNoFail = true;

            if (APSH_BlockActor* SpawnedBlock = GetWorld()->SpawnActor<APSH_BlockActor>(
                Data->actor, SpawnTransform, SpawnParams))
            {
                SpawnedBlock->SetOwner(this);
                SpawnedBlock->SetReplicates(true);
                SpawnedBlock->SetReplicateMovement(true);
                SpawnedBlock->SetOwnedByRoomInstance(true);
                SpawnedBlock->LoadBlockHierarchy(*Data, this);
                SpawnedActors.Add(SpawnedBlock);
                // PostInitializeComponents 전에 필요한 설정
                //SpawnedBlock->SetOwnedByRoomInstance(true);
                //if (SpawnedBlock->meshComp)
                //{
                //    SpawnedBlock->meshComp->SetSimulatePhysics(false);
                //}
                UE_LOG(LogTemp, Log, TEXT("Spawned root block: %s, Owner: %s"),
                    *SpawnedBlock->GetName(),
                    *SpawnedBlock->GetOwner()->GetName());
            }
        }
    }

    // 정리
    TempDataTable->RemoveFromRoot();
    TempDataTable = nullptr;

    // 플레이어 캐릭터 스폰
    for (APlayerController* PC : ConnectedPlayers)
    {
        SpawnAndSetupCharacter(PC);
    }

    PendingJsonData.Empty();
}

void AAutoRoomLevelInstance::SpawnAndSetupCharacter(APlayerController* PlayerController)
{
    if (!PlayerController || !HasAuthority())
    {
        return;
    }

    FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 200);
    FRotator SpawnRotation = FRotator::ZeroRotator;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = PlayerController;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    TSubclassOf<APSH_Player> CharacterClass = Cast<APSH_GameModeBase>(GetWorld()->GetAuthGameMode())->PlayerBasicClass;
    if (CharacterClass == nullptr) {
        UE_LOG(LogTemp, Warning, TEXT("csr _ fail"));
        return ;
    }
    if (APSH_Player* SpawnedCharacter = GetWorld()->SpawnActor<APSH_Player>(
        CharacterClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParams))
    {
        SpawnedActors.Add(SpawnedCharacter);
        PlayerController->Possess(SpawnedCharacter);
    }
}

void AAutoRoomLevelInstance::CleanupSpawnedActors()
{
    for (AActor* Actor : SpawnedActors)
    {
        if (IsValid(Actor))
        {
            if (APawn* Pawn = Cast<APawn>(Actor))
            {
                if (AController* Controller = Pawn->GetController())
                {
                    Controller->UnPossess();
                }
            }
            Actor->Destroy();
        }
    }
    SpawnedActors.Empty();
}

#pragma endregion


