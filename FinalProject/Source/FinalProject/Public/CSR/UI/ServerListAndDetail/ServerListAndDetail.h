// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerListAndDetail.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UServerListAndDetail : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
	class UServerListsWidget * WBP_Server_List;

    UPROPERTY(meta = (BindWidget))
	class URoomDetailPanel * Play_Detail;

	UFUNCTION(BlueprintCallable)
	void SendDetailData(const FString& RoomName, const FString& Title, int32 CurrentPlayer);
};
