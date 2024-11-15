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
    if (Btn_Save)
    {
        Btn_Save->OnClicked.AddDynamic(this, &UFirstSelect::OnBtn_SaveClicked);
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

// 액터 파괴시키기
void UFirstSelect::OnDestroyClicked()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("World is null"));
        return;
    }

    // 삭제할 블루프린트 클래스 경로 배열
    TArray<FStringClassReference> BlueprintClassPaths = {
        FStringClassReference(TEXT("/Game/YWK/BP/BP_MoveandFly.BP_MoveandFly_C")),
        FStringClassReference(TEXT("/Game/YWK/BP/BP_Rotate.BP_Rotate_C")),
        FStringClassReference(TEXT("/Game/PSH/PSH_BluePints/PSH_Actor/PSH_Block/BP_PSH_BlockActor.BP_PSH_BlockActor_C")),
        FStringClassReference(TEXT("/Game/PSH/PSH_BluePints/PSH_Actor/PSH_Block/BP_PSH_ConeActor.BP_PSH_ConeActor_C")),
        FStringClassReference(TEXT("/Game/PSH/PSH_BluePints/PSH_Actor/PSH_Block/BP_PSH_CylinderActor.BP_PSH_CylinderActor_C"))
    };

    // 각 경로에 해당하는 블루프린트 클래스 로드 및 액터 삭제
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


