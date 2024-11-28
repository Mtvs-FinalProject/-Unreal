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

    // Delegate�� ���ε��ϴ� �Լ�
    UFUNCTION()
    void HandleMoveButtonClicked();

    // ActionChoice ������ �����ϱ� ���� ����
    UActionChoice* ActionChoiceWidget;

//--------------------------------------------------------------------------------------------------------
    // �÷��̾� ������ üũ
    void CheckPlayerInput();

    // �÷��̾� ���� ���� Ȯ��
    void CheckPlayerMovement(float DeltaTime);

    // ���� �÷��̾� ������ �̵�
    void MoveInFrontOfPlayer();

    ACharacter* PlayerCharacter; // �÷��̾� ����
    FVector LastPlayerLocation; // �÷��̾��� ���� ��ġ
    // ���� �� ��� �÷���
    float PlayerStationaryTime; // �÷��̾ ���� �ð� ����
    bool bHasMovedToFront = false; // ���� �÷��̾� ������ �̵��ߴ��� ����
    bool bIsWaitingForPlayerToReachTarget = false; // ù ��° ��ǥ ��ġ�� �̵� ������ ����
    bool bIsWaitingForPlayerToReachNextTarget = false; // ���� ��ǥ ��ġ�� �̵� ������ ����
    bool bIsWaitingForBlockActor = false; // Ư�� ��� ���Ͱ� ��ȯ�Ǿ����� ��ٸ��� �÷���
    bool bIsWaitingForMoveAndFly = false; // Ư�� ��� ���Ͱ� ��ȯ�Ǿ����� ��ٸ��� �÷���
    bool bIsWaitingForMoveButtonClick = false;
    bool bIsWaitingForJumpPad = false; // JumpPad ������Ʈ Ȯ�� �÷���
    bool bIsWaitingForTwelfthTarget = false; 

    // Tick���� Ư�� �������Ʈ�� ã�� ���� �Լ�
    bool CheckForBlockActor();
    bool CheckForMoveAndFlyActor();
    bool CheckForJumpPadActor();

    void TeleportToPositionNearTarget(const FVector& TargetPosition, float OffsetDistance);
//-------------------------------------------------------------------------------------------------------
    // ù ��° ��� �ٲٱ�
    void FirstUpdateToNextRow();

    // �� ��° ��� �ٲٱ�
    void SecondUpdateToNextRow();

    // �� ��° ��� �ٲٱ�
    void ThirdUpdateToNextRow();

    // �� ��° ��� �ٲٱ�
    void FourthUpdateToNextRow();

    // �ټ� ��° ��� �ٲٱ�
    void FifthUpdateToNextRow();

    // ���� ��° ��� �ٲٱ�
    void SixthUpdateToNextRow();

    // �ϰ� ��° ��� �ٲٱ�
    void SeventhUpdateToNextRow();

    // ���� ��° ��� �ٲٱ�
    void eighthUpdateToNextRow();

    // ��ȩ ��° ��� �ٲٱ�
    void ninethUpdateToNextRow();

    // �� ��° ��� �ٲٱ�
    void TenthUpdateToNextRow();

    // �� �ѹ�° ��� �ٲٱ�
    void EleventhUpdateToNextRow();

    // �� �ι�° ��� �ٲٱ�
    void TwelfthUpdateToNextRow();

    // �� ����° ��� �ٲٱ�
    void ThirteenthUpdateToNextRow();

    // �� �׹�° ��� �ٲٱ�
    void FourteenthUpdateToNextRow();


};

