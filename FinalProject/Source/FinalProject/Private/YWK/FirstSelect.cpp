// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/FirstSelect.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "YWK/ActionChoice.h"
#include "PSH/PSH_UI/PSH_ObjectWidget.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "PSH/PSH_Player/PSH_PlayerController.h"
#include "PSH/PSH_Player/PSH_Player.h"
#include "GameFramework/SpringArmComponent.h"
#include "PSH/PSH_Actor/PSH_SpawnBot.h"

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
    if (Btn_Save)
    {
        Btn_Save->OnClicked.AddDynamic(this, &UFirstSelect::OnBtn_SaveClicked);
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
        APSH_PlayerController* PlayerController = Cast<APSH_PlayerController>(GetWorld()->GetFirstPlayerController());
        APSH_Player * player = Cast<APSH_Player>(PlayerController->GetPawn());
        if (PlayerController && player)
        {
            UE_LOG(LogTemp, Warning, TEXT("PlayerController is valid"));
           
            player->ToggleARmLength();
            player->SRPC_SpawnBotMoveTo();

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

// ���� �ı���Ű��
void UFirstSelect::OnDestroyClicked()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("World is null"));
        return;
    }

    // ������ �������Ʈ Ŭ���� ��� �迭
    TArray<FStringClassReference> BlueprintClassPaths = {
        FStringClassReference(TEXT("/Game/YWK/BP/BP_MoveandFly.BP_MoveandFly_C")),
        FStringClassReference(TEXT("/Game/YWK/BP/BP_Rotate.BP_Rotate_C")),
        FStringClassReference(TEXT("/Game/PSH/PSH_BluePints/PSH_Actor/PSH_Block/BP_PSH_BlockActor.BP_PSH_BlockActor_C")),
        FStringClassReference(TEXT("/Game/PSH/PSH_BluePints/PSH_Actor/PSH_Block/BP_PSH_ConeActor.BP_PSH_ConeActor_C")),
        FStringClassReference(TEXT("/Game/PSH/PSH_BluePints/PSH_Actor/PSH_Block/BP_PSH_CylinderActor.BP_PSH_CylinderActor_C"))
    };

    // �� ��ο� �ش��ϴ� �������Ʈ Ŭ���� �ε� �� ���� ����
    for (const FStringClassReference& ClassPath : BlueprintClassPaths)
    {
        UClass* TargetClass = ClassPath.TryLoadClass<AActor>();
        if (TargetClass)
        {
            TArray<AActor*> ActorsToDestroy;
            UGameplayStatics::GetAllActorsOfClass(World, TargetClass, ActorsToDestroy);

            for (AActor* Actor : ActorsToDestroy)
            {
                if (Actor)
                {
                    Actor->Destroy();
                    UE_LOG(LogTemp, Warning, TEXT("Destroyed Actor: %s"), *Actor->GetName());
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to load class for path: %s"), *ClassPath.ToString());
        }
    }
}

void UFirstSelect::OnBtn_SaveClicked()
{
    MapSave();
}


