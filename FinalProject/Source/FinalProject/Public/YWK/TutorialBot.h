#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ActionChoice.h" // ActionChoice
#include "Components/SceneComponent.h"
#include "Engine/DataTable.h"
#include "TutorialBot.generated.h"

UCLASS()
class FINALPROJECT_API ATutorialBot : public AActor
{
    GENERATED_BODY()

public:
    ATutorialBot();

    UFUNCTION(BlueprintImplementableEvent)
    void TutorialExit();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // Root Scene Component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* RootSceneComponent;

    // Cube Component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* CubeComponent;

    // Widget Component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWidgetComponent* WidgetComponent;

    // Tutorial Data Table
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
    UDataTable* TutorialDataTable;

    // Current Tutorial ID
    UPROPERTY(EditAnywhere)
    int32 CurrentID;

    // Widget Class
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
    TSubclassOf<class UTutorial_Widget> TutorialWidgetClass;

    // Tutorial Widget Instance
    UPROPERTY()
    class UTutorial_Widget* TutorialWidget;

    // Show Next Tutorial
    void ShowNextTutorial();

    FTimerHandle TimerHandle_UpdateTutorial;

    // Delegate를 바인딩하는 함수
    UFUNCTION()
    void HandleMoveButtonClicked();

    // ActionChoice 위젯을 추적하기 위한 변수
    UActionChoice* ActionChoiceWidget;

//--------------------------------------------------------------------------------------------------------
    // 플레이어 움직임 체크
    void CheckPlayerInput();

    // 플레이어 멈춤 상태 확인
    void CheckPlayerMovement(float DeltaTime);

    // 봇을 플레이어 앞으로 이동
    void MoveInFrontOfPlayer();

    ACharacter* PlayerCharacter; // 플레이어 참조
    FVector LastPlayerLocation; // 플레이어의 이전 위치
    // 상태 및 대기 플래그
    float PlayerStationaryTime; // 플레이어가 멈춘 시간 추적
    bool bHasMovedToFront = false; // 봇이 플레이어 앞으로 이동했는지 여부
    bool bIsWaitingForPlayerToReachTarget = false; // 첫 번째 목표 위치로 이동 중인지 여부
    bool bIsWaitingForPlayerToReachNextTarget = false; // 다음 목표 위치로 이동 중인지 여부
    bool bIsWaitingForBlockActor = false; // 특정 블록 액터가 소환되었는지 기다리는 플래그
    bool bIsWaitingForMoveAndFly = false; // 특정 기능 액터가 소환되었는지 기다리는 플래그
    bool bIsWaitingForMoveButtonClick = false;
    bool bIsWaitingForJumpPad = false; // JumpPad 오브젝트 확인 플래그
    bool bIsWaitingForTwelfthTarget = false; 

    // Tick에서 특정 블루프린트를 찾기 위한 함수
    bool CheckForBlockActor();
    bool CheckForMoveAndFlyActor();
    bool CheckForJumpPadActor();

    void TeleportToPositionNearTarget(const FVector& TargetPosition, float OffsetDistance);
//-------------------------------------------------------------------------------------------------------
    // 첫 번째 대사 바꾸기
    void FirstUpdateToNextRow();

    // 두 번째 대사 바꾸기
    void SecondUpdateToNextRow();

    // 세 번째 대사 바꾸기
    void ThirdUpdateToNextRow();

    // 네 번째 대사 바꾸기
    void FourthUpdateToNextRow();

    // 다섯 번째 대사 바꾸기
    void FifthUpdateToNextRow();

    // 여섯 번째 대사 바꾸기
    void SixthUpdateToNextRow();

    // 일곱 번째 대사 바꾸기
    void SeventhUpdateToNextRow();

    // 여덟 번째 대사 바꾸기
    void eighthUpdateToNextRow();

    // 아홉 번째 대사 바꾸기
    void ninethUpdateToNextRow();

    // 열 번째 대사 바꾸기
    void TenthUpdateToNextRow();

    // 열 한번째 대사 바꾸기
    void EleventhUpdateToNextRow();

    // 열 두번째 대사 바꾸기
    void TwelfthUpdateToNextRow();

    // 열 세번째 대사 바꾸기
    void ThirteenthUpdateToNextRow();

    // 열 네번째 대사 바꾸기
    void FourteenthUpdateToNextRow();


};

