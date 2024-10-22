// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/FirstSelect.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "YWK/ActionChoice.h"
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
                UE_LOG(LogTemp, Error, TEXT("Failed to create NewWidget"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("PlayerController is null"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ActionChoice is null"));
    }
}

void UFirstSelect::OnEndClicked()
{
	// ���� ���� ����
	RemoveFromParent();
}

