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
#include "PSH/PSH_Player/PSH_PlayerController.h"
#include "CSR/DedicatedServer/AutoGameState.h"
#include "../FinalProject.h"


AAutoRoomLevelInstance::AAutoRoomLevelInstance()
{
    bIsRoomAssigned = false;
    bIsLevelLoadedLocally = false;
    bReplicates = true;
    SetReplicatingMovement(true);
    bAlwaysRelevant = true;
    
    // 레벨 경로 설정 (프로젝트에 맞게 수정 필요)
    LevelPath = TEXT("/Game/Alpha/NewFolder/runningmap_alpha.runningmap_alpha");
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


void AAutoRoomLevelInstance::ReturnAllPlayersToLobby()
{
    // 현재 접속한 모든 플레이어들에 대해
    for (APlayerController* PC : ConnectedPlayers)
    {
        if (PC)
        {
            // 현재 Possess중인 Pawn 저장
            APawn* CurrentPawn = PC->GetPawn();

            // Unpossess
            PC->UnPossess();

            // Pawn 제거
            if (CurrentPawn)
            {
                CurrentPawn->Destroy();
            }

            // MainUI 표시 RPC 호출
            APSH_PlayerController* PSH_PC = Cast<APSH_PlayerController>(PC);
            if (PSH_PC)
            {
                PSH_PC->Client_ShowMainUI();
            }
            else {
                UE_LOG(LogTemp, Warning, TEXT("sibal"));
            }
        }
    }
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
        else {
            UE_LOG(LogTemp,Warning, TEXT("csr Im null"));
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
		RoomMode = ERoomMode::NoMode;
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

void AAutoRoomLevelInstance::ServerGetOutAll_Implementation()
{
    if (HasAuthority() && bIsRoomAssigned)
    {
        // 모든 플레이어를 로비로 되돌림
        ReturnAllPlayersToLobby();

        // 스폰된 엑터들 정리
        CleanupSpawnedActors();

        // 방 상태 초기화
        bIsRoomAssigned = false;
        ConnectedPlayers.Empty();
        SetCurrentRoomName(TEXT(""));

        // WorldAsset 제거
        SetRuntimeWorldAsset(nullptr);
        if (AAutoGameState* GS = GetWorld()->GetGameState<AAutoGameState>())
        {

            GS->UpdateRoomList();
        }

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
        UE_LOG(LogTemp, Warning, TEXT("ServerLeaveRoom_Implementation 1"));
        if (ConnectedPlayers.Remove(LeavingPlayer) > 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("ServerLeaveRoom_Implmentation 2"));

      
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
    UE_LOG(LogTemp, Warning, TEXT("csr ClientOnLeaveRoom_Implementation aaa"));
    // 자신이 나가는 플레이어인 경우만 레벨 언로드
    if (!HasAuthority()) {
        UE_LOG(LogTemp, Warning, TEXT("csr ClientOnLeaveRoom_Implementation bbb"));

        if (LeavingPlayer && LeavingPlayer->IsLocalController())
        {
            UE_LOG(LogTemp, Warning, TEXT("csr ClientOnLeaveRoom_Implementation ccc"));

            if (ULevelInstanceSubsystem* LevelInstanceSubsystem =
                GetWorld()->GetSubsystem<ULevelInstanceSubsystem>())
            {
                UE_LOG(LogTemp, Warning, TEXT("csr ClientOnLeaveRoom_Implementation ddd"));

                UE_LOG(LogTemp, Log, TEXT("Client %s: Unloading level after leaving"),
                    *LeavingPlayer->GetName());
                LevelInstanceSubsystem->RequestUnloadLevelInstance(this);
                bIsLevelLoadedLocally = false;
                APSH_PlayerController * player = Cast<APSH_PlayerController>(LeavingPlayer);
                if (player) {
                    player->Client_ShowMainUI();
                }
                else {
                    UE_LOG(LogTemp, Warning, TEXT("csr ClientOnLeaveRoom_Implementation ffffail"));

                }
                UE_LOG(LogTemp, Warning, TEXT("csr ClientOnLeaveRoom_Implementation"));
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

    UE_LOG(LogTemp, Warning, TEXT("csr csr %s"), *PendingJsonData);
    if (!HasAuthority() || PendingJsonData.IsEmpty())
    {
        return;
    }

    FPSH_WorldData WorldData = ParseJsonToWorldData(PendingJsonData);
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

    FPSH_WorldData WorldData = ParseJsonToWorldData(PendingJsonData);
    
    for (const FPSH_ObjectData& ObjectData : WorldData.BlockArray)
    {
        SpawnBlocksFromObjectData(ObjectData);
    }

    PendingJsonData.Empty();

}

void AAutoRoomLevelInstance::SpawnAndSetupCharacter(APlayerController* PlayerController)
{
    // 서버에서만 실행되도록 체크
    if (!PlayerController || !HasAuthority() || GetNetMode() == NM_Client)
    {
        return;
    }

    // 이미 폰을 가지고 있는지 체크
    if (PlayerController->GetPawn() != nullptr)
    {
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("csr who %s"), *PlayerController->GetName());
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
        UE_LOG(LogTemp, Warning, TEXT("csr SpawnedCharacter SpawnedCharacter"));
        APSH_PlayerController *player = Cast<APSH_PlayerController>(PlayerController);
        player->Client_deleteMainUI();
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

FPSH_WorldData AAutoRoomLevelInstance::ParseJsonToWorldData(const FString& JsonString)
{
    FPSH_WorldData WorldData;

    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    TSharedPtr<FJsonObject> JsonObject;

    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        PRINTLOG(TEXT("Failed to parse JSON string into WorldData."));
        return WorldData;
    }

    if (!JsonObject->HasField(TEXT("BlockArray")))
    {
        PRINTLOG(TEXT("JSON does not contain BlockArray field."));
        return WorldData;
    }

    TArray<TSharedPtr<FJsonValue>> BlockArray = JsonObject->GetArrayField(TEXT("BlockArray"));
    PRINTLOG(TEXT("Parsed BlockArray with %d elements."), BlockArray.Num());

    for (const TSharedPtr<FJsonValue>& BlockValue : BlockArray)
    {
        TSharedPtr<FJsonObject> BlockObject = BlockValue->AsObject();
        if (BlockObject.IsValid())
        {
            FPSH_BlockData BlockData = ConvertJsonObjectToBlockData(BlockObject);
            FPSH_ObjectData ObjectData;
            ObjectData.blockData = BlockData;

            WorldData.BlockArray.Add(ObjectData);

            PRINTLOG(TEXT("Successfully parsed block data: %s"), *BlockData.actor->GetName());
        }
        else
        {
            PRINTLOG(TEXT("Failed to parse block object in JSON."));
        }
    }

    return WorldData;
}

FString AAutoRoomLevelInstance::ConvertBlockDataToJson(const FPSH_ObjectData& BlockData)
{
    TSharedPtr<FJsonObject> RootJsonObject = MakeShared<FJsonObject>();

    // blockData를 JSON으로 변환
    TSharedPtr<FJsonObject> BlockDataJsonObject = ConvertBlockDataToJsonObject(BlockData.blockData);

    // 최상위 JSON 객체에 추가
    RootJsonObject->SetObjectField(TEXT("BlockData"), BlockDataJsonObject);
    RootJsonObject->SetBoolField(TEXT("bisSave"), BlockData.bisSave);

    // JSON 문자열로 직렬화
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    if (FJsonSerializer::Serialize(RootJsonObject.ToSharedRef(), Writer))
    {
        return OutputString;
    }

    return TEXT(""); // 실패 시 빈 문자열 반환
}

FString AAutoRoomLevelInstance::ConvertWorldDataToJson(const FPSH_WorldData& WorldData)
{
    TSharedRef<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

    TArray<TSharedPtr<FJsonValue>> BlockArray;
    for (const FPSH_ObjectData& BlockData : WorldData.BlockArray)
    {
        TSharedPtr<FJsonObject> BlockObject = ConvertBlockDataToJsonObject(BlockData.blockData);

        if (BlockObject.IsValid())
        {
            BlockArray.Add(MakeShareable(new FJsonValueObject(BlockObject)));
        }
        else
        {
            PRINTLOG(TEXT("Failed to convert block data to JSON object."));
        }
    }

    JsonObject->SetArrayField(TEXT("BlockArray"), BlockArray);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    if (FJsonSerializer::Serialize(JsonObject, Writer))
    {
        PRINTLOG(TEXT("Serialized WorldData JSON: %s"), *OutputString);
    }
    else
    {
        PRINTLOG(TEXT("Failed to serialize WorldData JSON."));
    }

    return OutputString;
}
TSharedPtr<FJsonObject> AAutoRoomLevelInstance::ConvertBlockDataToJsonObject(const FPSH_BlockData& BlockData)
{
    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

    // 기본 데이터 필드 추가
    JsonObject->SetStringField(TEXT("ActorClass"), BlockData.actor->GetPathName());
    JsonObject->SetStringField(TEXT("Transform"), BlockData.actorTransform.ToString());

    // funcitonData 배열 변환
    TArray<TSharedPtr<FJsonValue>> FunctionDataArray;
    for (const FPSH_FunctionBlockData& FunctionData : BlockData.funcitonData)
    {
        TSharedPtr<FJsonObject> FunctionDataObject = MakeShared<FJsonObject>();
        FunctionDataObject->SetArrayField(TEXT("IntArray"), ConvertIntArrayToJson(FunctionData.intArray));
        FunctionDataObject->SetArrayField(TEXT("FloatArray"), ConvertFloatArrayToJson(FunctionData.floatArray));
        FunctionDataObject->SetArrayField(TEXT("VectorArray"), ConvertVectorArrayToJson(FunctionData.fvectorArray));
        FunctionDataObject->SetArrayField(TEXT("RotatorArray"), ConvertRotatorArrayToJson(FunctionData.frotatorArray));
        FunctionDataObject->SetArrayField(TEXT("BoolArray"), ConvertBoolArrayToJson(FunctionData.boolArray));
        FunctionDataArray.Add(MakeShared<FJsonValueObject>(FunctionDataObject));
    }
    JsonObject->SetArrayField(TEXT("FunctionData"), FunctionDataArray);

    // childData 배열 변환
    TArray<TSharedPtr<FJsonValue>> ChildArray;
    for (const FPSH_BlockData& ChildData : BlockData.childData)
    {
        TSharedPtr<FJsonObject> ChildJsonObject = ConvertBlockDataToJsonObject(ChildData);
        if (ChildJsonObject.IsValid())
        {
            ChildArray.Add(MakeShareable(new FJsonValueObject(ChildJsonObject)));
        }
    }
    JsonObject->SetArrayField(TEXT("Children"), ChildArray);

    return JsonObject;
}
TArray<TSharedPtr<FJsonValue>> AAutoRoomLevelInstance::ConvertIntArrayToJson(const TArray<int32>& IntArray)
{
    TArray<TSharedPtr<FJsonValue>> JsonArray;
    for (int32 Value : IntArray)
    {
        JsonArray.Add(MakeShared<FJsonValueNumber>(Value));
    }
    return JsonArray;
}
TArray<TSharedPtr<FJsonValue>> AAutoRoomLevelInstance::ConvertFloatArrayToJson(const TArray<float>& FloatArray)
{
    TArray<TSharedPtr<FJsonValue>> JsonArray;
    for (float Value : FloatArray)
    {
        JsonArray.Add(MakeShared<FJsonValueNumber>(Value));
    }
    return JsonArray;
}
TArray<TSharedPtr<FJsonValue>> AAutoRoomLevelInstance::ConvertVectorArrayToJson(const TArray<FVector>& VectorArray)
{
    TArray<TSharedPtr<FJsonValue>> JsonArray;
    for (const FVector& Vector : VectorArray)
    {
        JsonArray.Add(MakeShared<FJsonValueString>(Vector.ToString()));
    }
    return JsonArray;
}
TArray<TSharedPtr<FJsonValue>> AAutoRoomLevelInstance::ConvertRotatorArrayToJson(const TArray<FRotator>& RotatorArray)
{
    TArray<TSharedPtr<FJsonValue>> JsonArray;
    for (const FRotator& Rotator : RotatorArray)
    {
        JsonArray.Add(MakeShared<FJsonValueString>(Rotator.ToString()));
    }
    return JsonArray;
}
TArray<TSharedPtr<FJsonValue>> AAutoRoomLevelInstance::ConvertBoolArrayToJson(const TArray<bool>& BoolArray)
{
    TArray<TSharedPtr<FJsonValue>> JsonArray;
    for (bool Value : BoolArray)
    {
        JsonArray.Add(MakeShared<FJsonValueBoolean>(Value));
    }
    return JsonArray;
}

TArray < int32 > AAutoRoomLevelInstance::ConvertJsonToIntArray(const TArray<TSharedPtr<FJsonValue>>& JsonArray)
{
    TArray<int32> IntArray;
    for (const TSharedPtr<FJsonValue>& Value : JsonArray)
    {
        IntArray.Add(Value->AsNumber());
    }
    return IntArray;
}
TArray<float> AAutoRoomLevelInstance::ConvertJsonToFloatArray(const TArray<TSharedPtr<FJsonValue>>& JsonArray)
{
    TArray<float> FloatArray;
    for (const TSharedPtr<FJsonValue>& Value : JsonArray)
    {
        FloatArray.Add(Value->AsNumber());
    }
    return FloatArray;
}
TArray<FVector> AAutoRoomLevelInstance::ConvertJsonToVectorArray(const TArray<TSharedPtr<FJsonValue>>& JsonArray)
{
    TArray<FVector> VectorArray;
    for (const TSharedPtr<FJsonValue>& Value : JsonArray)
    {
        FVector Vector;
        Vector.InitFromString(Value->AsString());
        VectorArray.Add(Vector);
    }

    return VectorArray;
}
TArray<FRotator> AAutoRoomLevelInstance::ConvertJsonToRotatorArray(const TArray<TSharedPtr<FJsonValue>>& JsonArray)
{
    TArray<FRotator> RotatorArray;
    for (const TSharedPtr<FJsonValue>& Value : JsonArray)
    {
        FRotator Rotator;
        Rotator.InitFromString(Value->AsString());
        RotatorArray.Add(Rotator);
    }
    return RotatorArray;
}
TArray<bool> AAutoRoomLevelInstance::ConvertJsonToBoolArray(const TArray<TSharedPtr<FJsonValue>>& JsonArray)
{
    TArray<bool> BoolArray;
    for (const TSharedPtr<FJsonValue>& Value : JsonArray)
    {
        BoolArray.Add(Value->AsBool());
    }
    return BoolArray;
}

void AAutoRoomLevelInstance::SpawnBlocksFromObjectData(const FPSH_ObjectData& ObjectData)
{
    APSH_BlockActor* RootBlock = SpawnBlock(ObjectData.blockData, nullptr);
    if (RootBlock)
    {
        SpawnedActors.Add(RootBlock);
    }
}

FPSH_BlockData AAutoRoomLevelInstance::ConvertJsonObjectToBlockData(const TSharedPtr<FJsonObject>& JsonObject)
{
    FPSH_BlockData BlockData;

    FString ActorClassPath;
    if (JsonObject->TryGetStringField(TEXT("ActorClass"), ActorClassPath))
    {
        BlockData.actor = LoadClass<AActor>(nullptr, *ActorClassPath);
    }

    FString TransformString;
    if (JsonObject->TryGetStringField(TEXT("Transform"), TransformString))
    {
        BlockData.actorTransform.InitFromString(TransformString);
    }

    const TArray<TSharedPtr<FJsonValue>>* FunctionDataArray;
    if (JsonObject->TryGetArrayField(TEXT("FunctionData"), FunctionDataArray))
    {
        for (const TSharedPtr<FJsonValue>& Value : *FunctionDataArray)
        {
            const TSharedPtr<FJsonObject>* FunctionDataObject;
            if (Value->TryGetObject(FunctionDataObject))
            {
                FPSH_FunctionBlockData FunctionData;
                // Populate FunctionData from FunctionDataObject
                BlockData.funcitonData.Add(FunctionData);
            }
        }
    }

    const TArray<TSharedPtr<FJsonValue>>* ChildrenArray;
    if (JsonObject->TryGetArrayField(TEXT("Children"), ChildrenArray))
    {
        for (const TSharedPtr<FJsonValue>& Value : *ChildrenArray)
        {
            const TSharedPtr<FJsonObject>* ChildObject;
            if (Value->TryGetObject(ChildObject))
            {
                BlockData.childData.Add(ConvertJsonObjectToBlockData(*ChildObject));
            }
        }
    }

    return BlockData;
}

FPSH_ObjectData AAutoRoomLevelInstance::ParseJsonToBlockData(const FString& JsonString)
{
    FPSH_ObjectData ObjectData;

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        const TSharedPtr<FJsonObject>* BlockDataObject;
        if (JsonObject->TryGetObjectField(TEXT("BlockData"), BlockDataObject))
        {
            ObjectData.blockData = ParseJsonObjectToBlockData(*BlockDataObject);
        }

        JsonObject->TryGetBoolField(TEXT("bisSave"), ObjectData.bisSave);
    }

    return ObjectData;
}

FPSH_BlockData AAutoRoomLevelInstance::ParseJsonObjectToBlockData(const TSharedPtr<FJsonObject>& JsonObject)
{
    return ConvertJsonObjectToBlockData(JsonObject);
}

void AAutoRoomLevelInstance::SpawnBlocksFromJson(const FString& JsonString)
{
    FPSH_WorldData WorldData = ParseJsonToWorldData(JsonString);

    for (const FPSH_ObjectData& ObjectData : WorldData.BlockArray)
    {
        SpawnBlocksFromObjectData(ObjectData);
    }
}

APSH_BlockActor* AAutoRoomLevelInstance::SpawnBlock(const FPSH_BlockData& BlockData, APSH_BlockActor* Parent)
{
    if (!BlockData.actor)
    {
        return nullptr;
    }

    FTransform SpawnTransform = BlockData.actorTransform;
    SpawnTransform.SetLocation(SpawnTransform.GetLocation() + GetActorLocation());

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.bNoFail = true;

    APSH_BlockActor* SpawnedBlock = GetWorld()->SpawnActor<APSH_BlockActor>(
        BlockData.actor, SpawnTransform, SpawnParams);

    if (SpawnedBlock)
    {
        SpawnedBlock->SetReplicates(true);
        SpawnedBlock->SetReplicateMovement(true);

        if (!BlockData.funcitonData.IsEmpty())
        {
            SpawnedBlock->ComponentLoadData(BlockData.funcitonData);
        }

        if (Parent)
        {
            SpawnedBlock->AttachToActor(Parent, FAttachmentTransformRules::KeepWorldTransform);
        }

        for (const FPSH_BlockData& ChildData : BlockData.childData)
        {
            SpawnBlock(ChildData, SpawnedBlock);
        }
    }

    return SpawnedBlock;
}

void AAutoRoomLevelInstance::ServerLoadTest_Implementation(const FString& JsonString)
{

}

