// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/ActionChoice.h"
#include "Components/Button.h"
#include "YWK/MyRotateActorComponent.h"
#include "YWK/MyFlyActorComponent.h"
#include "YWK/MyMoveActorComponent.h"

void UActionChoice::NativeConstruct()
{
	Super::NativeConstruct();
	
	//Move 버튼 눌렀을 때
	if (Btn_Move)
	{
		Btn_Move->OnClicked.AddDynamic(this, &UActionChoice::OnMoveClicked);
	}

	//Rotate 버튼 눌렀을 때
	if (Btn_Rotate)
	{
		Btn_Rotate->OnClicked.AddDynamic(this, &UActionChoice::OnRotateClicked);
	}

	//Fly 버튼 눌렀을 때
	if (Btn_Fly)
	{
		Btn_Fly->OnClicked.AddDynamic(this, &UActionChoice::OnFlyClicked);
	}

	//End 버튼 눌렀을 때
	if (End_Button)
	{
		End_Button->OnClicked.AddDynamic(this, &UActionChoice::OnEndClicked);
	}

	// Start 버튼 눌렀을 때
	if (Btn_StartAction)
	{
		Btn_StartAction->OnClicked.AddDynamic(this, &UActionChoice::OnStartActionClicked);
	}
}

void UActionChoice::OnMoveClicked()
{
	// 기존 UI 제거하기
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
	// 기존 UI 제거하기
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
	// 기존 UI 제거하기
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
	// 기존 UI 제거하기
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
			UE_LOG(LogTemp, Log, TEXT("FlyWidget creation failed"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("WBP_Fly is null"));
	}
}

void UActionChoice::SetControlledActor(AActor* NewActor)
{
	if (NewActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("ControlledActor is set to: %s"), *NewActor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("NewActor is null in SetControlledActor!"));
	}

	ControlledActor = NewActor;
}

void UActionChoice::OnStartActionClicked()
{

}
