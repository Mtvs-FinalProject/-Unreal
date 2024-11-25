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

 // 방 생성 함수 (수정됨: bool 반환 타입 추가)
    UFUNCTION(BlueprintCallable, Category = "Room")
    bool CreateAutoRoom(const FString& RoomName, APlayerController* RequestingPlayer);

    // 방 참가 함수
    void JoinAutoRoom(const FString& RoomName, APlayerController* RequestingPlayer);

    // 방 나가기 함수 
    void LeaveAutoRoom(const FString& RoomName, APlayerController* RequestingPlayer);

   // 사용 가능한 방 찾기
	UFUNCTION(BlueprintCallable, Category = "Room")
   AAutoRoomLevelInstance* FindAvailableAutoRoom() const;

   // 이름으로 방 찾기
   UFUNCTION(BlueprintCallable, Category = "Room")
   AAutoRoomLevelInstance* FindAutoRoomByName(const FString& RoomName) const;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

   // 풀 크기
   UPROPERTY(EditAnywhere, Category = "Room Configuration")
   int32 MaxRooms;

   // 방 사이 간격
   UPROPERTY(EditAnywhere, Category = "Room Configuration")
   float SpaceBetweenRooms;


   // 월드에 스폰하기 1
#pragma region
public:

private:

#pragma endregion

};

