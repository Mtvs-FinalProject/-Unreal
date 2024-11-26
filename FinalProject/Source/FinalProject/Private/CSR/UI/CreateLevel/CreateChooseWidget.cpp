// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/CreateLevel/CreateChooseWidget.h"

void UCreateChooseWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (SaveButton)
    {
        SaveButton->OnClicked.AddDynamic(this, &UCreateChooseWidget::HandleSaveButtonClicked);
    }
    if (CancleButton)
    {
        CancleButton->OnClicked.AddDynamic(this, &UCreateChooseWidget::HandleCancleButtonClicked);
    }
}

void UCreateChooseWidget::HandleSaveButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("UCreateChooseWidget::HandleSaveButtonClicked"));
    SaveButtonClick.Broadcast();
}

void UCreateChooseWidget::HandleCancleButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("UCreateChooseWidget::HandleCancleButtonClicked"));
    CancleButtonClick.Broadcast();
}
