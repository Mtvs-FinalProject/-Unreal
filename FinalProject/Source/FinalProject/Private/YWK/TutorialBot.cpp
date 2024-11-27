#include "YWK/TutorialBot.h"
#include "YWK/Tutorial_Widget.h"
#include "YWK/TutorialStruct.h" // FTutorialRow 포함
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

    // Root Scene Component 생성
    RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootSceneComponent;

    // Cube 컴포넌트 생성 및 루트에 부착
    CubeComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
    CubeComponent->SetupAttachment(RootComponent);

    // Widget Component 생성 및 루트에 부착
    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
    WidgetComponent->SetupAttachment(RootComponent);
    WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
    WidgetComponent->SetDrawSize(FVector2D(2000, 2000));
    WidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
}




void ATutorialBot::BeginPlay()
{
    Super::BeginPlay();

    // PlayerCharacter 초기화
    PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerCharacter is NULL!"));
    }
    else
    {
        LastPlayerLocation = PlayerCharacter->GetActorLocation();
    }

    // CubeComponent 및 WidgetComponent 유효성 검사
    if (!CubeComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("CubeComponent is NULL in BeginPlay."));
    }

    if (!WidgetComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("WidgetComponent is NULL in BeginPlay."));
    }

    // Tutorial Widget 초기화
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

    // ActionChoice Widget 설정
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

    // 첫 번째 목표 지점 확인
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
            bIsWaitingForPlayerToReachTarget = false; // 첫 번째 목표 완료
        }
    }

    // 두 번째 목표 지점 확인
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
            bIsWaitingForPlayerToReachNextTarget = false; // 두 번째 목표 완료
        }
    }

    // 세번째 목표 지점에 도달했는지 확인
    if (bIsWaitingForPlayerToReachTarget)
    {
        FVector PlayerLocation = PlayerCharacter->GetActorLocation();
        FVector2D CurrentLocation2D(PlayerLocation.X, PlayerLocation.Y);
        FVector2D FinalTargetLocation2D(5549.0f, -490.73482f); // 지점 좌표

        float Distance = FVector2D::Distance(CurrentLocation2D, FinalTargetLocation2D);

        if (Distance < 100.0f) // 허용 오차
        {
            UE_LOG(LogTemp, Log, TEXT("Player reached final target location. Proceeding to TenthUpdateToNextRow."));
            TenthUpdateToNextRow();
            bIsWaitingForPlayerToReachTarget = false; // 완료 플래그
        }
    }

    // 네번째 위치에 도달했는지 확인
    if (bIsWaitingForPlayerToReachNextTarget)
    {
        FVector PlayerLocation = PlayerCharacter->GetActorLocation();
        FVector2D CurrentLocation2D(PlayerLocation.X, PlayerLocation.Y);
        FVector2D TargetLocation2D(2740.3901f, -490.73482f); // 도달해야 할 좌표

        float Distance = FVector2D::Distance(CurrentLocation2D, TargetLocation2D);

        if (Distance < 100.0f) // 허용 오차
        {
            UE_LOG(LogTemp, Log, TEXT("Player reached target location (2740.3901, -490.73482). Proceeding to eleventhUpdateToNextRow."));
            EleventhUpdateToNextRow(); // 다음 단계 호출
            bIsWaitingForPlayerToReachNextTarget = false; // 플래그 초기화
        }
    }

    // 마지막 위치에 도달했는지 확인
    if (bIsWaitingForTwelfthTarget)
    {
        FVector PlayerLocation = PlayerCharacter->GetActorLocation();
        FVector2D CurrentLocation2D(PlayerLocation.X, PlayerLocation.Y);
        FVector2D TargetLocation2D(1857.20725f, -289.105003f); // 목표 좌표

        float Distance = FVector2D::Distance(CurrentLocation2D, TargetLocation2D);

        UE_LOG(LogTemp, Log, TEXT("Player Location: %s, Distance to Target: %f"), *PlayerLocation.ToString(), Distance);

        if (Distance < 100.0f) // 허용 오차
        {
            UE_LOG(LogTemp, Log, TEXT("Player reached Thirteenth target location."));
            ThirteenthUpdateToNextRow();
            bIsWaitingForTwelfthTarget = false; // 플래그 비활성화
        }
    }

    // 블록 액터 확인
    if (bIsWaitingForBlockActor && CheckForBlockActor())
    {
        UE_LOG(LogTemp, Log, TEXT("Block Actor found in the world."));
        FourthUpdateToNextRow();
        bIsWaitingForBlockActor = false; // 블록 액터 확인 완료
    }

    // 새로 추가된 MoveAndFly 오브젝트 확인
    if (bIsWaitingForMoveAndFly && CheckForMoveAndFlyActor())
    {
        UE_LOG(LogTemp, Log, TEXT("MoveAndFly Actor found. Moving to SeventhUpdateToNextRow."));
        SeventhUpdateToNextRow();
        bIsWaitingForMoveAndFly = false;
    }

    // JumpPad 오브젝트 확인
    if (bIsWaitingForJumpPad && CheckForJumpPadActor())
    {
        UE_LOG(LogTemp, Log, TEXT("JumpPad Actor found in the world. Proceeding to TwelfthUpdateToNextRow."));
        TwelfthUpdateToNextRow(); // 다음 단계 호출
        bIsWaitingForJumpPad = false; // 플래그 초기화
    }

    //// 플레이어와 TutorialBot 간 거리 계산
    //float DistanceToPlayer = FVector::Dist(PlayerCharacter->GetActorLocation(), GetActorLocation());

    //// 플레이어와의 거리가 1000 이상일 경우 순간이동
    //if (DistanceToPlayer > 1000.0f)
    //{
    //    FVector ForwardDirection = PlayerCharacter->GetActorForwardVector();
    //    FVector TargetLocation = PlayerCharacter->GetActorLocation() + ForwardDirection * 400.0f;

    //    SetActorLocation(TargetLocation);
    //    UE_LOG(LogTemp, Log, TEXT("TutorialBot teleported to: %s"), *TargetLocation.ToString());
    //}

    // TutorialBot이 플레이어를 바라보도록 설정
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        FVector PlayerCameraLocation;
        FRotator PlayerCameraRotation;
        PlayerController->GetPlayerViewPoint(PlayerCameraLocation, PlayerCameraRotation);

        FVector DirectionToPlayer = PlayerCameraLocation - GetActorLocation();
        DirectionToPlayer.Z = 0.0f; // Z축 회전을 무시하여 평면에서만 회전
        FRotator LookAtRotation = DirectionToPlayer.Rotation();
        SetActorRotation(FRotator(0.0f, LookAtRotation.Yaw, 0.0f)); // Yaw만 설정하여 바라보기
    }
}

// 인삿말 텍스트
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
        // 첫 번째 텍스트 업데이트
        TutorialWidget->UpdateText(RowArray[0]->Text);

        // 3초 뒤에 다음 텍스트로 변경
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle_UpdateTutorial, // 타이머 핸들
            this,                       // 호출 주체
            &ATutorialBot::FirstUpdateToNextRow, // 호출할 함수
            5.0f,                       // 딜레이 (초)
            false                       // 반복 여부 (여기선 한 번만 실행)
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
        bIsWaitingForMoveButtonClick = false; // 플래그 초기화
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
            // 플레이어 입력 감지 시 로그 출력 (필요에 따라 이벤트 추가 가능)
            UE_LOG(LogTemp, Log, TEXT("Player input detected!"));
        }
    }
}


//void ATutorialBot::CheckPlayerMovement(float DeltaTime)
//{
//    if (!PlayerCharacter) return; // PlayerCharacter가 NULL이면 실행하지 않음
//
//    FVector CurrentPlayerLocation = PlayerCharacter->GetActorLocation();
//
//    // 플레이어가 움직였는지 확인
//    if (FVector::Dist(CurrentPlayerLocation, LastPlayerLocation) > KINDA_SMALL_NUMBER)
//    {
//        // 플레이어가 움직였으면 멈춤 시간 초기화
//        PlayerStationaryTime = 0.0f;
//        LastPlayerLocation = CurrentPlayerLocation;
//        bHasMovedToFront = false; // 다시 움직이기 시작했으니 앞으로 이동 제어 플래그 리셋
//    }
//    else
//    {
//        // 플레이어가 멈췄으면 멈춘 시간 증가
//        PlayerStationaryTime += DeltaTime;
//
//        // 1초 이상 멈췄으면 봇을 플레이어 앞으로 이동
//        if (PlayerStationaryTime > 1.0f && !bHasMovedToFront)
//        {
//            MoveInFrontOfPlayer();
//            bHasMovedToFront = true; // 이미 앞으로 이동했음을 표시
//        }
//    }
//}
//
//
//
//void ATutorialBot::MoveInFrontOfPlayer()
//{
//    if (!PlayerCharacter) return; // PlayerCharacter가 NULL이면 실행하지 않음
//
//    FVector ForwardDirection = PlayerCharacter->GetActorForwardVector();
//    FVector TargetLocation = PlayerCharacter->GetActorLocation() + ForwardDirection * 400.0f;
//
//    SetActorLocation(TargetLocation);
//
//    // 멈춤 시간 초기화
//    PlayerStationaryTime = 0.0f;
//
//    UE_LOG(LogTemp, Log, TEXT("TutorialBot moved in front of player to: %s"), *TargetLocation.ToString());
//}


bool ATutorialBot::CheckForBlockActor()
{
    // 블루프린트 클래스를 런타임에 로드
    UClass* BlockActorClass = FindObject<UClass>(ANY_PACKAGE, TEXT("BP_PSH_BlockActor_C"));
    if (!BlockActorClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("BP_PSH_BlockActor_C class not found!"));
        return false;
    }


    // 해당 클래스를 가진 액터를 검색
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


// 새로 추가된 MoveAndFly 오브젝트 확인 함수
bool ATutorialBot::CheckForMoveAndFlyActor()
{
    // 블루프린트 클래스를 런타임에 로드
    UClass* MoveAndFlyActorClass = FindObject<UClass>(ANY_PACKAGE, TEXT("BP_MoveandFly_C"));
    if (!MoveAndFlyActorClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("BP_MoveandFly_C class not found!"));
        return false;
    }

    // 해당 클래스를 가진 액터를 검색
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
    // JumpPad 블루프린트 클래스 찾기
    UClass* JumpPadClass = FindObject<UClass>(ANY_PACKAGE, TEXT("BP_JumpPad_C"));
    if (!JumpPadClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("BP_JumpPad_C class not found!"));
        return false;
    }

    // 해당 클래스를 가진 액터 검색
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

// 플레이어 방향 체크 텍스트
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

        // 4초 뒤 플레이어 입력 체크로 SecondUpdateToNextRow 호출
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle_UpdateTutorial,
            this,
            &ATutorialBot::SecondUpdateToNextRow,
            4.0f,
            false
        );
    }
}

// 첫 목표지점 이동 시
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

        // 플레이어가 특정 위치로 이동했는지 확인하도록 플래그 설정
        bIsWaitingForPlayerToReachTarget = true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SecondUpdateToNextRow: RowArray does not have enough rows! Current count: %d"), RowArray.Num());
    }
}

// 오브젝트 생성시 텍스트(여기서 tutorial bot 순간이동)
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

        // 튜토리얼 봇 순간이동
        FVector TargetPosition(10097.166284f, -224.637443f, GetActorLocation().Z);
        SetActorLocation(TargetPosition);
        UE_LOG(LogTemp, Log, TEXT("Tutorial Bot moved to First Target Position: %s"), *TargetPosition.ToString());

        // BP_PSH_BlockActor0 존재 여부를 기다리기 시작
        bIsWaitingForBlockActor = true;
    }
}

// 두 번째 목표지점까지 가기 텍스트
void ATutorialBot::FourthUpdateToNextRow()
{
    if (!TutorialDataTable)
    {
        return;
    }

    static const FString ContextString(TEXT("Tutorial Context"));

    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    // RowArray[4] 출력
    if (RowArray.Num() > 4)
    {
        TutorialWidget->UpdateText(RowArray[4]->Text);
        UE_LOG(LogTemp, Log, TEXT("FourthUpdateToNextRow: Updated text to RowArray[4]: %s"), *RowArray[4]->Text);

        // 두 번째 목표 지점 확인 플래그 활성화
        bIsWaitingForPlayerToReachNextTarget = true;
    }
}





// 두 번째 목표 지점에 도착했을 때 나올 텍스트(두 번째 튜토리얼 봇 순간이동)
void ATutorialBot::FifthUpdateToNextRow()
{
    if (!TutorialDataTable)
    {
        return;
    }

    static const FString ContextString(TEXT("Tutorial Context"));

    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    // RowArray[5] 출력
    if (RowArray.Num() > 5)
    {
        TutorialWidget->UpdateText(RowArray[5]->Text);
        UE_LOG(LogTemp, Log, TEXT("FifthUpdateToNextRow: Updated text to RowArray[5]: %s"), *RowArray[5]->Text);

        // 튜토리얼 봇 순간이동
        FVector TargetPosition(7517.254219f, -224.637443f, GetActorLocation().Z);
        SetActorLocation(TargetPosition);
        UE_LOG(LogTemp, Log, TEXT("Tutorial Bot moved to Second Target Position: %s"), *TargetPosition.ToString());

        // 4초 뒤에 SixthUpdateToNextRow 호출
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle_UpdateTutorial,
            this,
            &ATutorialBot::SixthUpdateToNextRow,
            4.0f,
            false
        );
    }
}


// 5번 텍스트 나오고 4초 뒤에 6번째 텍스트 나오게
void ATutorialBot::SixthUpdateToNextRow()
{
    if (!TutorialDataTable)
    {
        return;
    }

    static const FString ContextString(TEXT("Tutorial Context"));

    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    // RowArray[6] 출력
    if (RowArray.Num() > 6)
    {
        TutorialWidget->UpdateText(RowArray[6]->Text);
        UE_LOG(LogTemp, Log, TEXT("SixthUpdateToNextRow: Updated text to RowArray[6]: %s"), *RowArray[6]->Text);

        // MoveAndFly 액터가 생성되었는지 확인하도록 플래그 설정
        bIsWaitingForMoveAndFly = true;
        UE_LOG(LogTemp, Log, TEXT("SixthUpdateToNextRow: Waiting for MoveAndFly Actor."));
    }
}


// move object가 생성되었다면
void ATutorialBot::SeventhUpdateToNextRow()
{
    if (!TutorialDataTable)
    {
        return;
    }

    static const FString ContextString(TEXT("Tutorial Context"));

    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    // RowArray[7] 출력
    if (RowArray.Num() > 7)
    {
        TutorialWidget->UpdateText(RowArray[7]->Text);
        UE_LOG(LogTemp, Log, TEXT("SeventhUpdateToNextRow: Updated text to RowArray[7]: %s"), *RowArray[7]->Text);

        // 4초 뒤에 SixthUpdateToNextRow 호출
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle_UpdateTutorial, // 타이머 핸들
            this,                       // 호출 주체
            &ATutorialBot::eighthUpdateToNextRow, // 호출할 함수
            4.0f,                       // 딜레이 (초)
            false );                      // 반복 실행 여부
    }
}

// moveobject를 조립하고
void ATutorialBot::eighthUpdateToNextRow()
{
    if (!TutorialDataTable)
    {
        return;
    }

    static const FString ContextString(TEXT("Tutorial Context"));

    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    // RowArray[8] 출력
    if (RowArray.Num() > 8)
    {
        TutorialWidget->UpdateText(RowArray[8]->Text);
        UE_LOG(LogTemp, Log, TEXT("EighthUpdateToNextRow: Updated text to RowArray[8]: %s"), *RowArray[8]->Text);

        // 6초 뒤에 ninethUpdateToNextRow 호출
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle_UpdateTutorial, // 타이머 핸들
            this,                       // 호출 주체
            &ATutorialBot::ninethUpdateToNextRow, // 호출할 함수
            8.0f,                       // 딜레이 (초)
            false);                      // 반복 실행 여부
    }
}

// 해당 지점에 도착하면 넘어가기
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

    // RowArray[9] 출력
    if (RowArray.Num() > 9)
    {
        TutorialWidget->UpdateText(RowArray[9]->Text);
        UE_LOG(LogTemp, Log, TEXT("ninethUpdateToNextRow: Updated text to RowArray[9]: %s"), *RowArray[9]->Text);

        // 다음 조건 확인을 위한 플래그 설정
        bIsWaitingForPlayerToReachTarget = true; // 특정 지점 도달 감시
        UE_LOG(LogTemp, Log, TEXT("ninethUpdateToNextRow: Waiting for player to reach final target."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ninethUpdateToNextRow: RowArray does not have enough entries."));
    }
}



// 여기가 세 번째 순간이동
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

    // RowArray[10] 출력
    if (RowArray.Num() > 10)
    {
        TutorialWidget->UpdateText(RowArray[10]->Text);
        UE_LOG(LogTemp, Log, TEXT("TenthUpdateToNextRow: Updated text to RowArray[10]: %s"), *RowArray[10]->Text);

        // 튜토리얼 봇 순간이동
        FVector TargetPosition(4942.90665f, -224.637443f, GetActorLocation().Z);
        SetActorLocation(TargetPosition);
        UE_LOG(LogTemp, Log, TEXT("Tutorial Bot moved to Third Target Position: %s"), *TargetPosition.ToString());

        // 다음 좌표 도달 조건 감시를 위한 플래그 설정
        bIsWaitingForPlayerToReachNextTarget = true;
        UE_LOG(LogTemp, Log, TEXT("TenthUpdateToNextRow: Waiting for player to reach target at (2740.3901, -490.73482)."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("TenthUpdateToNextRow: RowArray does not have enough entries."));
    }
}

// 네 번째 순간이동
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

    // RowArray[11] 출력
    if (RowArray.Num() > 11)
    {
        TutorialWidget->UpdateText(RowArray[11]->Text);
        UE_LOG(LogTemp, Log, TEXT("EleventhUpdateToNextRow: Updated text to RowArray[11]: %s"), *RowArray[11]->Text);

        // 튜토리얼 봇 순간이동
        FVector TargetPosition(2482.541584f, 103.554918f, GetActorLocation().Z);
        SetActorLocation(TargetPosition);
        UE_LOG(LogTemp, Log, TEXT("Tutorial Bot moved to Fourth Target Position: %s"), *TargetPosition.ToString());

        // JumpPad 오브젝트 확인을 위한 플래그 활성화
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

        // 좌표 체크 플래그 활성화
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

        // 튜토리얼 봇 순간이동
        FVector TargetPosition(1403.678482f, -294.131452f, 1104.997603f);
        SetActorLocation(TargetPosition);
        UE_LOG(LogTemp, Log, TEXT("Tutorial Bot moved to Final Target Position: %s"), *TargetPosition.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ThirteenthUpdateToNextRow: RowArray does not have enough entries."));
    }
}

// 마지막 순간이동
void ATutorialBot::FourteenthUpdateToNextRow()
{
    if (!TutorialDataTable)
    {
        return;
    }

    static const FString ContextString(TEXT("Tutorial Context"));

    TArray<FTutorialStruct*> RowArray;
    TutorialDataTable->GetAllRows<FTutorialStruct>(ContextString, RowArray);

    // RowArray[14] 출력
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

