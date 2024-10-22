// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/ActionChoice.h"
#include "Components/Button.h"

void UActionChoice::NativeConstruct()
{
	Super::NativeConstruct();
	
	//Move ��ư ������ ��
	if (Btn_Move)
	{
		Btn_Move->OnClicked.AddDynamic(this, &UActionChoice::OnMoveClicked);
	}

	//Rotate ��ư ������ ��
	if (Btn_Rotate)
	{
		Btn_Rotate->OnClicked.AddDynamic(this, &UActionChoice::OnRotateClicked);
	}

	//Fly ��ư ������ ��
	if (Btn_Fly)
	{
		Btn_Fly->OnClicked.AddDynamic(this, &UActionChoice::OnFlyClicked);
	}

	//End ��ư ������ ��
	if (End_Button)
	{
		End_Button->OnClicked.AddDynamic(this, &UActionChoice::OnEndClicked);
	}
}

void UActionChoice::OnMoveClicked()
{
	// ���� UI �����ϱ�
	RemoveFromParent();

	if (WBP_Move)
	{
		UUserWidget* MoveWidget = CreateWidget<UUserWidget>(GetWorld(), WBP_Move);
		if (MoveWidget)
		{
			MoveWidget->AddToViewport();
		}
	}
}

void UActionChoice::OnRotateClicked()
{
	// ���� UI �����ϱ�
	RemoveFromParent();

	if (WBP_Rotate)
	{
		UUserWidget* RotateWidget = CreateWidget<UUserWidget>(GetWorld(), WBP_Rotate);
		if (RotateWidget)
		{
			RotateWidget->AddToViewport();
		}
	}
}

void UActionChoice::OnEndClicked()
{
	// ���� UI �����ϱ�
	RemoveFromParent();

	if (FirstSelect)
	{
		UUserWidget* NewWidget = CreateWidget<UUserWidget>(GetWorld(), FirstSelect);
		if (NewWidget)
		{
			NewWidget->AddToViewport();
		}
	}
}

void UActionChoice::OnFlyClicked()
{
	// ���� UI �����ϱ�
	RemoveFromParent();

	if (WBP_Fly)
	{
		UE_LOG(LogTemp, Warning, TEXT("WBP_Fly is valid"));
		UUserWidget* FlyWidget = CreateWidget<UUserWidget>(GetWorld(), WBP_Fly);
		if (FlyWidget)
		{
			FlyWidget->AddToViewport();
			UE_LOG(LogTemp, Warning, TEXT("FlyWidget added to viewport"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FlyWidget creation failed"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WBP_Fly is null"));
	}
}
