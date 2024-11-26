// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/ServerListAndDetail/ServerListsWidget.h"
#include "CSR/UI/ServerListAndDetail/ModalWidget.h"
#include "Kismet/GameplayStatics.h"
#include "CSR/UI/ServerListAndDetail/ServerListItem/OpenServerList.h"
#include "PSH/PSH_Player/PSH_PlayerController.h"
#include "CSR/UI/ServerListAndDetail/ServerListItem/OpenServerItem.h"

void UServerListsWidget::NativeConstruct()
{
    if (!ModalWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("ModalWidgetClass is not set!"));
        return;
    }

    if (Create_BTN)
    {
        Create_BTN->OnClicked.AddDynamic(this, &UServerListsWidget::OnCreateButtonClicked);
    }

    if (Join_BTN)
    {
        Join_BTN->OnClicked.AddDynamic(this, &UServerListsWidget::OnJoinButtonClicked);
    }
}

void UServerListsWidget::NativeDestruct()
{
    Super::NativeDestruct();

    if (ActiveModalWidget)
    {
        ActiveModalWidget->RemoveFromParent();
        ActiveModalWidget = nullptr;
    }
}

void UServerListsWidget::OnCreateButtonClicked()
{
    if (ModalWidgetClass)
    {
        // 모달 위젯 생성
        ActiveModalWidget = CreateWidget<UModalWidget>(GetWorld(), ModalWidgetClass);
        if (ActiveModalWidget)
        {
            ActiveModalWidget->AddToViewport();

            // 입력 모드 UI로 전환
            if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
            {
                FInputModeUIOnly InputMode;
                InputMode.SetWidgetToFocus(ActiveModalWidget->TakeWidget());
                PlayerController->SetInputMode(InputMode);
                PlayerController->bShowMouseCursor = true;
            }
        }
    }
}

void UServerListsWidget::OnJoinButtonClicked()
{
    UOpenServerItem * Item = WBP_Open_Server_List->GetSelectedItem();
    if (Item) {
        FString RoomName = Item->GetRoomName();
        if (APSH_PlayerController* PC = Cast<APSH_PlayerController>(GetOwningPlayer()))
        {
            PC->ServerRequestJoinAutoRoom(RoomName);
            RemoveFromParent();
        }
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("yet select any things"));
    }
}

