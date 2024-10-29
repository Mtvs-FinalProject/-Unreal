// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/FirstSelect.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "YWK/ActionChoice.h"
#include "PSH/PSH_UI/PSH_ObjectWidget.h"
#include "GameFramework/PlayerController.h"

void UFirstSelect::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 액션버튼 눌렀을 때
	if (Btn_Action)
	{
		Btn_Action->OnClicked.AddDynamic(this, &UFirstSelect::OnActionClicked);
	}

	// end 버튼 눌렀을 때
	if (Btn_end)
	{
		Btn_end->OnClicked.AddDynamic(this, &UFirstSelect::OnEndClicked);
	}

    // Craft 버튼 눌렀을 때
    if (Btn_Craft)
    {
        Btn_Craft->OnClicked.AddDynamic(this, &UFirstSelect::OnCraftClicked);
    }

    // Destroy 버튼 눌렀을 때
    if (Btn_Destroy)
    {
        Btn_Destroy->OnClicked.AddDynamic(this, &UFirstSelect::OnDestroyClicked);
    }

    bIsEditMode = false;
}

void UFirstSelect::OnActionClicked()
{
    
    // 현재 위젯 제거
    RemoveFromParent();
    UE_LOG(LogTemp, Warning, TEXT("RemoveFromParent called"));

    if (ActionChoice)
    {
        // 위젯 주인 가져오기
        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

        if (PlayerController)
        {
            UE_LOG(LogTemp, Warning, TEXT("PlayerController is valid"));

            // 위젯 생성
            UUserWidget* NewWidget = CreateWidget<UUserWidget>(PlayerController, ActionChoice);
            if (NewWidget)
            {
                // 새 위젯 Viewport에 추가
                NewWidget->AddToViewport();
                UE_LOG(LogTemp, Warning, TEXT("NewWidget added to viewport"));
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("Failed to create NewWidget"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("PlayerController is null"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("ActionChoice is null"));
    }
}

void UFirstSelect::OnEndClicked()
{
	// 현재 위젯 제거
	RemoveFromParent();
}

void UFirstSelect::OnCraftClicked()
{
    // 현재 위젯 제거
    RemoveFromParent();

    // Object widget열기
    if (PSH_ObjectWidget)
    {
        // 위젯 주인 가져오기
        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

        if (PlayerController)
        {
            UE_LOG(LogTemp, Warning, TEXT("PlayerController is valid"));

            // 위젯 생성
            UUserWidget* NewObjectWidget = CreateWidget<UUserWidget>(PlayerController, PSH_ObjectWidget);
            if (NewObjectWidget)
            {
                // 새 위젯 Viewport에 추가
                NewObjectWidget->AddToViewport();
                UE_LOG(LogTemp, Warning, TEXT("NewWidget added to viewport"));
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("Failed to create NewWidget"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("PlayerController is null"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("PSH_ObjectWidget is null"));
    }

}

void UFirstSelect::OnDestroyClicked()
{
    
}

