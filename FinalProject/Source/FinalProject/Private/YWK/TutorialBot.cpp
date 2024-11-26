#include "YWK/TutorialBot.h"
#include "YWK/Tutorial_Widget.h"
#include "YWK/TutorialStruct.h" // FTutorialRow ����
#include "YWK/ActionChoice.h"
#include "PSH/PSH_Player/PSH_Player.h"
#include "Engine/DataTable.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATutorialBot::ATutorialBot()
{
    PrimaryActorTick.bCanEverTick = true;

    CurrentID = 1;

    // Root Scene Component ����
    RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootSceneComponent;

    // Cube ������Ʈ ���� �� ��Ʈ�� ����
    CubeComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
    CubeComponent->SetupAttachment(RootComponent);

    // Widget Component ���� �� ��Ʈ�� ����
    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
    WidgetComponent->SetupAttachment(RootComponent);
    WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
    WidgetComponent->SetDrawSize(FVector2D(2000, 2000));
    WidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
}




void ATutorialBot::BeginPlay()
{
    Super::BeginPlay();

    // PlayerCharacter �ʱ�ȭ
    PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerCharacter is NULL!"));
    }
    else
    {
        LastPlayerLocation = PlayerCharacter->GetActorLocation();
    }

    // CubeComponent �� WidgetComponent ��ȿ�� �˻�
    if (!CubeComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("CubeComponent is NULL in BeginPlay."));
    }

    if (!WidgetComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("WidgetComponent is NULL in BeginPlay."));
    }

    // Tutorial Widget �ʱ�ȭ
    if (WidgetComponent && TutorialWidgetClass)
    {
        WidgetComponent->SetWidgetClass(TutorialWidgetClass);

        this->TutorialWidget = Cast<UTutorial_Widget>(WidgetComponent->GetUserWidgetObject());
        if (this->TutorialWidget)
        {
            this->TutorialWidget->TutorialBot = this;
            ShowNextTutorial();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to cast UserWidgetObject to UTutorial_Widget."));
        }
    }

    // ActionChoice Widget ����
    TArray<AActor*> FoundWidgets;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), UActionChoice::StaticClass(), FoundWidgets);
}


void ATutorialBot::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter is null!"));
        return;
    }

    // ù ��° ��ǥ ���� Ȯ��
    if (bIsWaitingForPlayerToReachTarget)
    {
        FVector PlayerLocation = PlayerCharacter->GetActorLocation();
        FVector2D CurrentLocation2D(PlayerLocation.X, PlayerLocation.Y);
        FVector2D TargetLocation2D(10498.0f, -491.0f);

        float Distance = FVector2D::Distance(CurrentLocation2D, TargetLocation2D);

        if (Distance < 100.0f)
        {
            UE_LOG(LogTemp, Log, TEXT("Player reached first target location."));
            ThirdUpdateToNextRow();
            bIsWaitingForPlayerToReachTarget = false; // ù ��° ��ǥ �Ϸ�
        }
    }

    // �� ��° ��ǥ ���� Ȯ��
    if (bIsWaitingForPlayerToReachNextTarget)
    {
        FVector PlayerLocation = PlayerCharacter->GetActorLocation();
        FVector2D CurrentLocation2D(PlayerLocation.X, PlayerLocation.Y);
        FVector2D NewTargetLocation2D(8051.115888f, -490.73482f);

        float Distance = FVector2D::Distance(CurrentLocation2D, NewTargetLocation2D);

        if (Distance < 100.0f)
        {
            UE_LOG(LogTemp, Log, TEXT("Player reached second target location."));
            FifthUpdateToNextRow();
            bIsWaitingForPlayerToReachNextTarget = false; // �� ��° ��ǥ �Ϸ�
        }
    }

    // ����° ��ǥ ������ �����ߴ��� Ȯ��
    if (bIsWaitingForPlayerToReachTarget)
    {
        FVector PlayerLocation = PlayerCharacter->GetActorLocation();
        FVector2D CurrentLocation2D(PlayerLocation.X, PlayerLocation.Y);
        FVector2D FinalTargetLocation2D(5549.0f, -490.73482f); // ���� ��ǥ

        float Distance = FVector2D::Distance(CurrentLocation2D, FinalTargetLocation2D);

        if (Distance < 100.0f) // ��� ����
        {
            UE_LOG(LogTemp, Log, TEXT("Player reached final target location. Proceeding to TenthUpdateToNextRow."));
            TenthUpdateToNextRow();
            bIsWaitingForPlayerToReachTarget = false; // �Ϸ� �÷���
        }
    }

    // �׹�° ��ġ�� �����ߴ��� Ȯ��
    if (bIsWaitingForPlayerToReachNextTarget)
    {
        FVector PlayerLocation = PlayerCharacter->GetActorLocation();
        FVector2D CurrentLocation2D(PlayerLocation.X, PlayerLocation.Y);
        FVector2D TargetLocation2D(2740.3901f, -490.73482f); // �����ؾ� �� ��ǥ

        float Distance = FVector2D::Distance(CurrentLocation2D, TargetLocation2D);

        if (Distance < 100.0f) // ��� ����
        {
            UE_LOG(LogTemp, Log, TEXT("Player reached target location (2740.3901, -490.73482). Proceeding to eleventhUpdateToNextRow."));
            EleventhUpdateToNextRow(); // ���� �ܰ� ȣ��
            bIsWaitingForPlayerToReachNextTarget = false; // �÷��� �ʱ�ȭ
        }
    }

    // ������ ��ġ�� �����ߴ��� Ȯ��
    if (bIsWaitingForTwelfthTarget)
    {
        FVector PlayerLocation = PlayerCharacter->GetActorLocation();
        FVector2D CurrentLocation2D(PlayerLocation.X, PlayerLocation.Y);
        FVector2D TargetLocation2D(1857.20725f, -289.105003f); // ��ǥ ��ǥ

        float Distance = FVector2D::Distance(CurrentLocation2D, TargetLocation2D);

        UE_LOG(LogTemp, Log, TEXT("Player Location: %s, Distance to Target: %f"), *PlayerLocation.ToString(), Distance);

        if (Distance < 100.0f) // ��� ����
        {
            UE_LOG(LogTemp, Log, TEXT("Player reached Thirteenth target location."));
            ThirteenthUpdateToNextRow();
            bIsWaitingForTwelfthTarget = false; // �÷��� ��Ȱ��ȭ
        }
    }

    // ��� ���� Ȯ��
    if (bIsWaitingForBlockActor && CheckForBlockActor())
    {
        UE_LOG(LogTemp, Log, TEXT("Block Actor found in the world."));
        FourthUpdateToNextRow();
        bIsWaitingForBlockActor = false; // ��� ���� Ȯ�� �Ϸ�
    }

    // ���� �߰��� MoveAndFly ������Ʈ Ȯ��
    if (bIsWaitingForMoveAndFly && CheckForMoveAndFlyActor())
    {
        UE_LOG(LogTemp, Log, TEXT("MoveAndFly Actor found. Moving to SeventhUpdateToNextRow."));
        SeventhUpdateToNextRow();
        bIsWaitingForMoveAndFly = false;
    }

    // JumpPad ������Ʈ Ȯ��
    if (bIsWaitingForJumpPad && CheckForJumpPadActor())
    {
        UE_LOG(LogTemp, Log, TEXT("JumpPad Actor found in the world. Proceeding to TwelfthUpdateToNextRow."));
        TwelfthUpdateToNextRow(); // ���� �ܰ� ȣ��
        bIsWaitingForJumpPad = false; // �÷��� �ʱ�ȭ
    }

    //// �÷��̾�� TutorialBot �� �Ÿ� ���
    //float DistanceToPlayer = FVector::Dist(PlayerCharacter->GetActorLocation(), GetActorLocation());

    //// �÷��̾���� �Ÿ��� 1000 �̻��� ��� �����̵�
    //if (DistanceToPlayer > 1000.0f)
    //{
    //    FVector ForwardDirection = PlayerCharacter->GetActorForwardVector();
    //    FVector TargetLocation = PlayerCharacter->GetActorLocation() + ForwardDirection * 400.0f;

    //    SetActorLocation(TargetLocation);
    //    UE_LOG(LogTemp, Log, TEXT("TutorialBot teleported to: %s"), *TargetLocation.ToString());
    //}

    // TutorialBot�� �÷��̾ �ٶ󺸵��� ����
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        FVector PlayerCameraLocation;
        FRotator PlayerCameraRotation;
        PlayerController->GetPlayerViewPoint(PlayerCameraLocation, PlayerCameraRotation);

        FVector DirectionToPlayer = PlayerCameraLocation - GetActorLocation();
        DirectionToPlayer.Z = 0.0f; // Z�� ȸ���� �����Ͽ� ��鿡���� ȸ��
        FRotator LookAtRotation = DirectionToPlayer.Rotation();
        SetActorRotation(FRotator(0.0f, LookAtRotation.Yaw, 0.0f)); // Yaw�� �����Ͽ� �ٶ󺸱�
    }
}

// �λ� �ؽ�Ʈ
void ATutorialBot::ShowNextTutorial()
{
    if (!TutorialDataTable)
    {
        return;
    }
    static const FString ContextString(TEXT("Tutorial Context"));

    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    if (RowArray.IsEmpty() == false)
    {
        // ù ��° �ؽ�Ʈ ������Ʈ
        TutorialWidget->UpdateText(RowArray[0]->Text);

        // 3�� �ڿ� ���� �ؽ�Ʈ�� ����
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle_UpdateTutorial, // Ÿ�̸� �ڵ�
            this,                       // ȣ�� ��ü
            &ATutorialBot::FirstUpdateToNextRow, // ȣ���� �Լ�
            5.0f,                       // ������ (��)
            false                       // �ݺ� ���� (���⼱ �� ���� ����)
        );
    }
}


void ATutorialBot::HandleMoveButtonClicked()
{
    UE_LOG(LogTemp, Log, TEXT("HandleMoveButtonClicked called."));

    if (bIsWaitingForMoveButtonClick)
    {
        UE_LOG(LogTemp, Log, TEXT("Move button clicked while waiting. Proceeding to ninethUpdateToNextRow."));
        ninethUpdateToNextRow();
        bIsWaitingForMoveButtonClick = false; // �÷��� �ʱ�ȭ
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Move button clicked but not waiting for it."));
    }
}


void ATutorialBot::CheckPlayerInput()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (PlayerController->IsInputKeyDown(EKeys::W) ||
            PlayerController->IsInputKeyDown(EKeys::A) ||
            PlayerController->IsInputKeyDown(EKeys::S) ||
            PlayerController->IsInputKeyDown(EKeys::D))
        {
            // �÷��̾� �Է� ���� �� �α� ��� (�ʿ信 ���� �̺�Ʈ �߰� ����)
            UE_LOG(LogTemp, Log, TEXT("Player input detected!"));
        }
    }
}


//void ATutorialBot::CheckPlayerMovement(float DeltaTime)
//{
//    if (!PlayerCharacter) return; // PlayerCharacter�� NULL�̸� �������� ����
//
//    FVector CurrentPlayerLocation = PlayerCharacter->GetActorLocation();
//
//    // �÷��̾ ���������� Ȯ��
//    if (FVector::Dist(CurrentPlayerLocation, LastPlayerLocation) > KINDA_SMALL_NUMBER)
//    {
//        // �÷��̾ ���������� ���� �ð� �ʱ�ȭ
//        PlayerStationaryTime = 0.0f;
//        LastPlayerLocation = CurrentPlayerLocation;
//        bHasMovedToFront = false; // �ٽ� �����̱� ���������� ������ �̵� ���� �÷��� ����
//    }
//    else
//    {
//        // �÷��̾ �������� ���� �ð� ����
//        PlayerStationaryTime += DeltaTime;
//
//        // 1�� �̻� �������� ���� �÷��̾� ������ �̵�
//        if (PlayerStationaryTime > 1.0f && !bHasMovedToFront)
//        {
//            MoveInFrontOfPlayer();
//            bHasMovedToFront = true; // �̹� ������ �̵������� ǥ��
//        }
//    }
//}
//
//
//
//void ATutorialBot::MoveInFrontOfPlayer()
//{
//    if (!PlayerCharacter) return; // PlayerCharacter�� NULL�̸� �������� ����
//
//    FVector ForwardDirection = PlayerCharacter->GetActorForwardVector();
//    FVector TargetLocation = PlayerCharacter->GetActorLocation() + ForwardDirection * 400.0f;
//
//    SetActorLocation(TargetLocation);
//
//    // ���� �ð� �ʱ�ȭ
//    PlayerStationaryTime = 0.0f;
//
//    UE_LOG(LogTemp, Log, TEXT("TutorialBot moved in front of player to: %s"), *TargetLocation.ToString());
//}


bool ATutorialBot::CheckForBlockActor()
{
    // �������Ʈ Ŭ������ ��Ÿ�ӿ� �ε�
    UClass* BlockActorClass = FindObject<UClass>(ANY_PACKAGE, TEXT("BP_PSH_BlockActor_C"));
    if (!BlockActorClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("BP_PSH_BlockActor_C class not found!"));
        return false;
    }


    // �ش� Ŭ������ ���� ���͸� �˻�
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), BlockActorClass, FoundActors);

    if (FoundActors.Num() > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("BP_PSH_BlockActor found in the world."));
        return true;
    }

    UE_LOG(LogTemp, Warning, TEXT("BP_PSH_BlockActor not found in the world."));
    return false;
}


// ���� �߰��� MoveAndFly ������Ʈ Ȯ�� �Լ�
bool ATutorialBot::CheckForMoveAndFlyActor()
{
    // �������Ʈ Ŭ������ ��Ÿ�ӿ� �ε�
    UClass* MoveAndFlyActorClass = FindObject<UClass>(ANY_PACKAGE, TEXT("BP_MoveandFly_C"));
    if (!MoveAndFlyActorClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("BP_MoveandFly_C class not found!"));
        return false;
    }

    // �ش� Ŭ������ ���� ���͸� �˻�
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), MoveAndFlyActorClass, FoundActors);

    if (FoundActors.Num() > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("BP_MoveandFly found in the world."));
        return true;
    }

    UE_LOG(LogTemp, Warning, TEXT("BP_MoveandFly not found in the world."));
    return false;
}

bool ATutorialBot::CheckForJumpPadActor()
{
    // JumpPad �������Ʈ Ŭ���� ã��
    UClass* JumpPadClass = FindObject<UClass>(ANY_PACKAGE, TEXT("BP_JumpPad_C"));
    if (!JumpPadClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("BP_JumpPad_C class not found!"));
        return false;
    }

    // �ش� Ŭ������ ���� ���� �˻�
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), JumpPadClass, FoundActors);

    if (FoundActors.Num() > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("BP_JumpPad found in the world."));
        return true;
    }

    UE_LOG(LogTemp, Warning, TEXT("BP_JumpPad not found in the world."));
    return false;
}


void ATutorialBot::TeleportToPositionNearTarget(const FVector& TargetPosition, float OffsetDistance)
{

}

// �÷��̾� ���� üũ �ؽ�Ʈ
void ATutorialBot::FirstUpdateToNextRow()
{
    if (!TutorialDataTable)
    {
        return;
    }
    static const FString ContextString(TEXT("Tutorial Context"));

    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    if (RowArray.Num() > 1)
    {
        TutorialWidget->UpdateText(RowArray[1]->Text);

        // 4�� �� �÷��̾� �Է� üũ�� SecondUpdateToNextRow ȣ��
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle_UpdateTutorial,
            this,
            &ATutorialBot::SecondUpdateToNextRow,
            4.0f,
            false
        );
    }
}

// ù ��ǥ���� �̵� ��
void ATutorialBot::SecondUpdateToNextRow()
{
    if (!TutorialDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("TutorialDataTable is null in SecondUpdateToNextRow!"));
        return;
    }

    static const FString ContextString(TEXT("Tutorial Context"));
    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    if (RowArray.Num() > 2)
    {
        TutorialWidget->UpdateText(RowArray[2]->Text);
        UE_LOG(LogTemp, Warning, TEXT("SecondUpdateToNextRow: Updated text to RowArray[2]: %s"), *RowArray[2]->Text);

        // �÷��̾ Ư�� ��ġ�� �̵��ߴ��� Ȯ���ϵ��� �÷��� ����
        bIsWaitingForPlayerToReachTarget = true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SecondUpdateToNextRow: RowArray does not have enough rows! Current count: %d"), RowArray.Num());
    }
}

// ������Ʈ ������ �ؽ�Ʈ(���⼭ tutorial bot �����̵�)
void ATutorialBot::ThirdUpdateToNextRow()
{
    if (!TutorialDataTable)
    {
        return;
    }
    static const FString ContextString(TEXT("Tutorial Context"));

    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    if (RowArray.Num() > 3)
    {
        TutorialWidget->UpdateText(RowArray[3]->Text);
        UE_LOG(LogTemp, Log, TEXT("ThirdUpdateToNextRow: Updated text to RowArray[3]: %s"), *RowArray[3]->Text);

        // Ʃ�丮�� �� �����̵�
        FVector TargetPosition(10097.166284f, -224.637443f, GetActorLocation().Z);
        SetActorLocation(TargetPosition);
        UE_LOG(LogTemp, Log, TEXT("Tutorial Bot moved to First Target Position: %s"), *TargetPosition.ToString());

        // BP_PSH_BlockActor0 ���� ���θ� ��ٸ��� ����
        bIsWaitingForBlockActor = true;
    }
}

// �� ��° ��ǥ�������� ���� �ؽ�Ʈ
void ATutorialBot::FourthUpdateToNextRow()
{
    if (!TutorialDataTable)
    {
        return;
    }

    static const FString ContextString(TEXT("Tutorial Context"));

    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    // RowArray[4] ���
    if (RowArray.Num() > 4)
    {
        TutorialWidget->UpdateText(RowArray[4]->Text);
        UE_LOG(LogTemp, Log, TEXT("FourthUpdateToNextRow: Updated text to RowArray[4]: %s"), *RowArray[4]->Text);

        // �� ��° ��ǥ ���� Ȯ�� �÷��� Ȱ��ȭ
        bIsWaitingForPlayerToReachNextTarget = true;
    }
}





// �� ��° ��ǥ ������ �������� �� ���� �ؽ�Ʈ(�� ��° Ʃ�丮�� �� �����̵�)
void ATutorialBot::FifthUpdateToNextRow()
{
    if (!TutorialDataTable)
    {
        return;
    }

    static const FString ContextString(TEXT("Tutorial Context"));

    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    // RowArray[5] ���
    if (RowArray.Num() > 5)
    {
        TutorialWidget->UpdateText(RowArray[5]->Text);
        UE_LOG(LogTemp, Log, TEXT("FifthUpdateToNextRow: Updated text to RowArray[5]: %s"), *RowArray[5]->Text);

        // Ʃ�丮�� �� �����̵�
        FVector TargetPosition(7517.254219f, -224.637443f, GetActorLocation().Z);
        SetActorLocation(TargetPosition);
        UE_LOG(LogTemp, Log, TEXT("Tutorial Bot moved to Second Target Position: %s"), *TargetPosition.ToString());

        // 4�� �ڿ� SixthUpdateToNextRow ȣ��
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle_UpdateTutorial,
            this,
            &ATutorialBot::SixthUpdateToNextRow,
            4.0f,
            false
        );
    }
}


// 5�� �ؽ�Ʈ ������ 4�� �ڿ� 6��° �ؽ�Ʈ ������
void ATutorialBot::SixthUpdateToNextRow()
{
    if (!TutorialDataTable)
    {
        return;
    }

    static const FString ContextString(TEXT("Tutorial Context"));

    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    // RowArray[6] ���
    if (RowArray.Num() > 6)
    {
        TutorialWidget->UpdateText(RowArray[6]->Text);
        UE_LOG(LogTemp, Log, TEXT("SixthUpdateToNextRow: Updated text to RowArray[6]: %s"), *RowArray[6]->Text);

        // MoveAndFly ���Ͱ� �����Ǿ����� Ȯ���ϵ��� �÷��� ����
        bIsWaitingForMoveAndFly = true;
        UE_LOG(LogTemp, Log, TEXT("SixthUpdateToNextRow: Waiting for MoveAndFly Actor."));
    }
}


// move object�� �����Ǿ��ٸ�
void ATutorialBot::SeventhUpdateToNextRow()
{
    if (!TutorialDataTable)
    {
        return;
    }

    static const FString ContextString(TEXT("Tutorial Context"));

    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    // RowArray[7] ���
    if (RowArray.Num() > 7)
    {
        TutorialWidget->UpdateText(RowArray[7]->Text);
        UE_LOG(LogTemp, Log, TEXT("SeventhUpdateToNextRow: Updated text to RowArray[7]: %s"), *RowArray[7]->Text);

        // 4�� �ڿ� SixthUpdateToNextRow ȣ��
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle_UpdateTutorial, // Ÿ�̸� �ڵ�
            this,                       // ȣ�� ��ü
            &ATutorialBot::eighthUpdateToNextRow, // ȣ���� �Լ�
            4.0f,                       // ������ (��)
            false );                      // �ݺ� ���� ����
    }
}

// moveobject�� �����ϰ�
void ATutorialBot::eighthUpdateToNextRow()
{
    if (!TutorialDataTable)
    {
        return;
    }

    static const FString ContextString(TEXT("Tutorial Context"));

    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    // RowArray[8] ���
    if (RowArray.Num() > 8)
    {
        TutorialWidget->UpdateText(RowArray[8]->Text);
        UE_LOG(LogTemp, Log, TEXT("EighthUpdateToNextRow: Updated text to RowArray[8]: %s"), *RowArray[8]->Text);

        // 6�� �ڿ� ninethUpdateToNextRow ȣ��
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle_UpdateTutorial, // Ÿ�̸� �ڵ�
            this,                       // ȣ�� ��ü
            &ATutorialBot::ninethUpdateToNextRow, // ȣ���� �Լ�
            8.0f,                       // ������ (��)
            false);                      // �ݺ� ���� ����
    }
}

// �ش� ������ �����ϸ� �Ѿ��
void ATutorialBot::ninethUpdateToNextRow()
{
    if (!TutorialDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("TutorialDataTable is NULL in ninethUpdateToNextRow."));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("ninethUpdateToNextRow called."));

    static const FString ContextString(TEXT("Tutorial Context"));

    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    // RowArray[9] ���
    if (RowArray.Num() > 9)
    {
        TutorialWidget->UpdateText(RowArray[9]->Text);
        UE_LOG(LogTemp, Log, TEXT("ninethUpdateToNextRow: Updated text to RowArray[9]: %s"), *RowArray[9]->Text);

        // ���� ���� Ȯ���� ���� �÷��� ����
        bIsWaitingForPlayerToReachTarget = true; // Ư�� ���� ���� ����
        UE_LOG(LogTemp, Log, TEXT("ninethUpdateToNextRow: Waiting for player to reach final target."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ninethUpdateToNextRow: RowArray does not have enough entries."));
    }
}



// ���Ⱑ �� ��° �����̵�
void ATutorialBot::TenthUpdateToNextRow()
{
    if (!TutorialDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("TutorialDataTable is NULL in TenthUpdateToNextRow."));
        return;
    }

    static const FString ContextString(TEXT("Tutorial Context"));

    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    // RowArray[10] ���
    if (RowArray.Num() > 10)
    {
        TutorialWidget->UpdateText(RowArray[10]->Text);
        UE_LOG(LogTemp, Log, TEXT("TenthUpdateToNextRow: Updated text to RowArray[10]: %s"), *RowArray[10]->Text);

        // Ʃ�丮�� �� �����̵�
        FVector TargetPosition(4942.90665f, -224.637443f, GetActorLocation().Z);
        SetActorLocation(TargetPosition);
        UE_LOG(LogTemp, Log, TEXT("Tutorial Bot moved to Third Target Position: %s"), *TargetPosition.ToString());

        // ���� ��ǥ ���� ���� ���ø� ���� �÷��� ����
        bIsWaitingForPlayerToReachNextTarget = true;
        UE_LOG(LogTemp, Log, TEXT("TenthUpdateToNextRow: Waiting for player to reach target at (2740.3901, -490.73482)."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("TenthUpdateToNextRow: RowArray does not have enough entries."));
    }
}

// �� ��° �����̵�
void ATutorialBot::EleventhUpdateToNextRow()
{
    if (!TutorialDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("TutorialDataTable is NULL in EleventhUpdateToNextRow."));
        return;
    }

    static const FString ContextString(TEXT("Tutorial Context"));

    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    // RowArray[11] ���
    if (RowArray.Num() > 11)
    {
        TutorialWidget->UpdateText(RowArray[11]->Text);
        UE_LOG(LogTemp, Log, TEXT("EleventhUpdateToNextRow: Updated text to RowArray[11]: %s"), *RowArray[11]->Text);

        // Ʃ�丮�� �� �����̵�
        FVector TargetPosition(2482.541584f, 103.554918f, GetActorLocation().Z);
        SetActorLocation(TargetPosition);
        UE_LOG(LogTemp, Log, TEXT("Tutorial Bot moved to Fourth Target Position: %s"), *TargetPosition.ToString());

        // JumpPad ������Ʈ Ȯ���� ���� �÷��� Ȱ��ȭ
        bIsWaitingForJumpPad = true;
        UE_LOG(LogTemp, Log, TEXT("EleventhUpdateToNextRow: Waiting for JumpPad Actor in the world."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("EleventhUpdateToNextRow: RowArray does not have enough entries."));
    }
}


void ATutorialBot::TwelfthUpdateToNextRow()
{
    if (!TutorialDataTable)
    {
        return;
    }

    static const FString ContextString(TEXT("Tutorial Context"));

    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    if (RowArray.Num() > 12)
    {
        TutorialWidget->UpdateText(RowArray[12]->Text);
        UE_LOG(LogTemp, Log, TEXT("TwelfthUpdateToNextRow: Updated text to RowArray[12]: %s"), *RowArray[12]->Text);

        // ��ǥ üũ �÷��� Ȱ��ȭ
        bIsWaitingForTwelfthTarget = true;
        UE_LOG(LogTemp, Log, TEXT("TwelfthUpdateToNextRow: Waiting for player to reach Thirteenth target."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("TwelfthUpdateToNextRow: RowArray does not have enough entries."));
    }
}



void ATutorialBot::ThirteenthUpdateToNextRow()
{
    if (!TutorialDataTable)
    {
        return;
    }

    static const FString ContextString(TEXT("Tutorial Context"));

    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    if (RowArray.Num() > 13)
    {
        TutorialWidget->UpdateText(RowArray[13]->Text);
        UE_LOG(LogTemp, Log, TEXT("ThirteenthUpdateToNextRow: Updated text to RowArray[13]: %s"), *RowArray[13]->Text);

        // Ʃ�丮�� �� �����̵�
        FVector TargetPosition(1403.678482f, -294.131452f, 1104.997603f);
        SetActorLocation(TargetPosition);
        UE_LOG(LogTemp, Log, TEXT("Tutorial Bot moved to Final Target Position: %s"), *TargetPosition.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ThirteenthUpdateToNextRow: RowArray does not have enough entries."));
    }
}

// ������ �����̵�
void ATutorialBot::FourteenthUpdateToNextRow()
{
    if (!TutorialDataTable)
    {
        return;
    }

    static const FString ContextString(TEXT("Tutorial Context"));

    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    // RowArray[14] ���
    if (RowArray.Num() > 14)
    {
        TutorialWidget->UpdateText(RowArray[14]->Text);
        UE_LOG(LogTemp, Log, TEXT("FourteenthUpdateToNextRow: Updated text to RowArray[14]: %s"), *RowArray[14]->Text);

    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("FourteenthUpdateToNextRow: RowArray does not have enough entries."));
    }
}

