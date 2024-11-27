// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/MyChoiceActionWidget.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"

void UMyChoiceActionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
	if (Btn_Move)
	{
		Btn_Move->OnClicked.AddDynamic(this, &UMyChoiceActionWidget::OnMoveButtonClicked);
	}

	if (Btn_Fly)
	{
		Btn_Fly->OnClicked.AddDynamic(this, &UMyChoiceActionWidget::OnFlyButtonClicked);
	}
}

void UMyChoiceActionWidget::OnMoveButtonClicked()
{
	// 기존 UI제거
	SetVisibility(ESlateVisibility::Hidden);
	if (WBP_Move)
	{
		UUserWidget* MoveWidget = CreateWidget<UUserWidget>(GetWorld(), WBP_Move);
		if (MoveWidget)
		{
			MoveWidget->AddToViewport();
		}
	}
}

void UMyChoiceActionWidget::OnFlyButtonClicked()
{
	SetVisibility(ESlateVisibility::Hidden);
	if (WBP_Fly)
	{
		UUserWidget* FlyWidget = CreateWidget<UUserWidget>(GetWorld(), WBP_Fly);
		if (FlyWidget)
		{
			FlyWidget->AddToViewport();
		}
	}
}
