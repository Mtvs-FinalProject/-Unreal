// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "CSR/DedicatedServer/AutoGameState.h"
#include "UObject/NoExportTypes.h"
#include "OpenServerItem.generated.h"

class UOpenServerList;
/**
 * 
 */
UCLASS()
class FINALPROJECT_API UOpenServerItem : public UUserWidget
{
	GENERATED_BODY()

public:
    UOpenServerItem(const FObjectInitializer& ObjectInitializer);

    void SetRoomInfo(const FRoomListInfo& InRoomInfo);
    void SetSelected(bool bSelected);
    const FString& GetRoomName() const { return CurrentRoomName; }
    ERoomMode GetRoomMode() const { return CurrentMode; }

    UFUNCTION()
    void SetParentWidget(class UOpenServerList* ParentWidget_) {
        this->ParentWidget = ParentWidget_;
    }
protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UButton* MainButton;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* RoomNameText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ModeText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* PlayerCountText;

    UPROPERTY(EditAnywhere, Category = "Style")
    FLinearColor NormalColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

    UPROPERTY(EditAnywhere, Category = "Style")
    FLinearColor SelectedColor = FLinearColor(0.2f, 0.7f, 0.2f, 1.0f);

    UFUNCTION()
    void OnMainButtonClicked();

    UPROPERTY()
    class UOpenServerList *ParentWidget;
private:
    FString CurrentRoomName;
    ERoomMode CurrentMode;
    bool bIsSelected;
};
