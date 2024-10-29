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
	
	// �׼ǹ�ư ������ ��
	if (Btn_Action)
	{
		Btn_Action->OnClicked.AddDynamic(this, &UFirstSelect::OnActionClicked);
	}

	// end ��ư ������ ��
	if (Btn_end)
	{
		Btn_end->OnClicked.AddDynamic(this, &UFirstSelect::OnEndClicked);
	}

    // Craft ��ư ������ ��
    if (Btn_Craft)
    {
        Btn_Craft->OnClicked.AddDynamic(this, &UFirstSelect::OnCraftClicked);
    }

    // Destroy ��ư ������ ��
    if (Btn_Destroy)
    {
        Btn_Destroy->OnClicked.AddDynamic(this, &UFirstSelect::OnDestroyClicked);
    }

    bIsEditMode = false;
}

void UFirstSelect::OnActionClicked()
{
    
    // ���� ���� ����
    RemoveFromParent();
    UE_LOG(LogTemp, Warning, TEXT("RemoveFromParent called"));

    if (ActionChoice)
    {
        // ���� ���� ��������
        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

        if (PlayerController)
        {
            UE_LOG(LogTemp, Warning, TEXT("PlayerController is valid"));

            // ���� ����
            UUserWidget* NewWidget = CreateWidget<UUserWidget>(PlayerController, ActionChoice);
            if (NewWidget)
            {
                // �� ���� Viewport�� �߰�
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
	// ���� ���� ����
	RemoveFromParent();
}

void UFirstSelect::OnCraftClicked()
{
    // ���� ���� ����
    RemoveFromParent();

    // Object widget����
    if (PSH_ObjectWidget)
    {
        // ���� ���� ��������
        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

        if (PlayerController)
        {
            UE_LOG(LogTemp, Warning, TEXT("PlayerController is valid"));

            // ���� ����
            UUserWidget* NewObjectWidget = CreateWidget<UUserWidget>(PlayerController, PSH_ObjectWidget);
            if (NewObjectWidget)
            {
                // �� ���� Viewport�� �߰�
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

