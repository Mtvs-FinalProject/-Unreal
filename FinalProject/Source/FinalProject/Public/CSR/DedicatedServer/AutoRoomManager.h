// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AutoRoomLevelInstance.h"
#include "AutoGameState.h"
#include "AutoRoomManager.generated.h"

UCLASS()
class FINALPROJECT_API AAutoRoomManager : public AActor
{
	GENERATED_BODY()
	
public:
    AAutoRoomManager();

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
    TArray<AAutoRoomLevelInstance*> AutoRoomPool;

 // �� ���� �Լ� (������: bool ��ȯ Ÿ�� �߰�)
    UFUNCTION(BlueprintCallable, Category = "Room")
    bool CreateAutoRoom(const FString& RoomName, APlayerController* RequestingPlayer);

    // �� ���� �Լ�
    void JoinAutoRoom(const FString& RoomName, APlayerController* RequestingPlayer);

    // �� ������ �Լ� 
    void LeaveAutoRoom(const FString& RoomName, APlayerController* RequestingPlayer);

   // ��� ������ �� ã��
	UFUNCTION(BlueprintCallable, Category = "Room")
   AAutoRoomLevelInstance* FindAvailableAutoRoom() const;

   // �̸����� �� ã��
   UFUNCTION(BlueprintCallable, Category = "Room")
   AAutoRoomLevelInstance* FindAutoRoomByName(const FString& RoomName) const;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

   // Ǯ ũ��
   UPROPERTY(EditAnywhere, Category = "Room Configuration")
   int32 MaxRooms;

   // �� ���� ����
   UPROPERTY(EditAnywhere, Category = "Room Configuration")
   float SpaceBetweenRooms;


   // ���忡 �����ϱ� 1
#pragma region
public:

private:

#pragma endregion

};

