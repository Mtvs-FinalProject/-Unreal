// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/ServerListAndDetail/ServerListsWidget.h"
#include "CSR/UI/ServerListAndDetail/ModalWidget.h"
#include "Kismet/GameplayStatics.h"

void UServerListsWidget::NativeConstruct()
{
    if (!ModalWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("ModalWidgetClass is not set!"));
        return;
    }

    if (UButton* CreateButton = Cast<UButton>(GetWidgetFromName(TEXT("Create_BTN"))))
    {
        CreateButton->OnClicked.AddDynamic(this, &UServerListsWidget::OnCreateButtonClicked);
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
