// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "CSR/DedicatedServer/AutoGameState.h"
#include "Components/VerticalBox.h"
#include "OpenServerList.generated.h"

class UOpenServerItem;


UCLASS()
class FINALPROJECT_API UOpenServerList : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
	void SendDetailData(const FString& RoomName, const FString& Title, int32 CurrentPlayer) const;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    // 방 아이템 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UOpenServerItem> ServerItemClass;

    // 방 목록을 표시할 Vertical Box
    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* RoomListBox;

    // 현재 선택된 아이템
    UPROPERTY()
    UOpenServerItem* SelectedItem;

    // GameState의 방 목록 업데이트 이벤트에 반응하는 함수
    UFUNCTION()
    void OnRoomListUpdated();

    // 방 목록 새로고침
    void RefreshRoomList();

public:
    // 아이템 선택 처리 함수
    void SelectItem(UOpenServerItem* NewSelectedItem);

    // 현재 선택된 아이템 반환
    UOpenServerItem* GetSelectedItem() const { return SelectedItem; }

private:
    // 이벤트 바인딩 해제를 위한 약한 참조
    TWeakObjectPtr<AAutoGameState> CachedGameState;
};