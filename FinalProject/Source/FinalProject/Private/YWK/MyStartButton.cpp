#include "YWK/MyStartButton.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"
#include "EngineUtils.h"
#include "PSH/PSH_Player/PSH_Player.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "YWK/FlyWidget.h"
#include "YWK/Movewidget.h"
#include "YWK/RotationWidget.h"
#include "GameFramework/Actor.h"
#include "YWK/MyFlyActorComponent.h"
#include "YWK/MyMoveActorComponent.h"
#include "YWK/MyRotateActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/EditableText.h"
#include "PSH/PSH_GameMode/PSH_GameModeBase.h"
#include "../FinalProject.h"

AMyStartButton::AMyStartButton()
{
    PrimaryActorTick.bCanEverTick = true;

    RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
    RootComponent = RootSceneComponent;

    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
    BaseMesh->SetupAttachment(RootSceneComponent);

    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    ButtonMesh->SetupAttachment(BaseMesh);

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    CollisionBox->SetupAttachment(RootSceneComponent);
    CollisionBox->SetCollisionProfileName(TEXT("Trigger"));
    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AMyStartButton::OnOverlapBegin);
    CollisionBox->OnComponentEndOverlap.AddDynamic(this, &AMyStartButton::OnOverlapEnd);

    OriginalScale = FVector(0.7f, 0.7f, 1.4f);
    PressedScale = FVector(OriginalScale.X, OriginalScale.Y, 0.05f);
}

void AMyStartButton::BeginPlay()
{
    Super::BeginPlay();
    OriginalScale = ButtonMesh->GetComponentScale();

    TArray<UUserWidget*> FoundWidgets;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, URotationWidget::StaticClass(), false);

    if (FoundWidgets.Num() > 0)
    {
        RotationWidgetInstance = Cast<URotationWidget>(FoundWidgets[0]);
    }
}

void AMyStartButton::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}


// 충돌 시 기능 활성화
void AMyStartButton::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    FVector CurrentScale = ButtonMesh->GetComponentScale();
    ButtonMesh->SetWorldScale3D(FVector(CurrentScale.X, CurrentScale.Y, PressedScale.Z));

    if (Cast<APSH_Player>(OtherActor))
    {
        SetOwner(Cast<APSH_Player>(OtherActor));
    }

    if (HasAuthority())
    {
       APSH_GameModeBase * GM = Cast<APSH_GameModeBase>(GetWorld()->GetAuthGameMode());

       if (GM)
       {
           bmodeChange = !bmodeChange;
           GM->SetCreateingCheck(bmodeChange);
           PRINTLOG(TEXT("SetCreateingCheck"));
       }
    }

//     // RotationWidgetInstance가 null인 경우 위젯을 찾음
//     if (!RotationWidgetInstance)
//     {
//         TArray<UUserWidget*> FoundRotationWidgets;
//         UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundRotationWidgets, URotationWidget::StaticClass(), false);
// 
//         if (FoundRotationWidgets.Num() > 0)
//         {
//             RotationWidgetInstance = Cast<URotationWidget>(FoundRotationWidgets[0]);
//         }
//     }
// 
//     // MovewidgetInstance가 null인 경우 위젯을 찾고 초기화
//     if (!MovewidgetInstance)
//     {
//         TArray<UUserWidget*> FoundMoveWidgets;
//         UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundMoveWidgets, UMovewidget::StaticClass(), false);
// 
//         if (FoundMoveWidgets.Num() > 0)
//         {
//             MovewidgetInstance = Cast<UMovewidget>(FoundMoveWidgets[0]);
//         }
//     }
// 
//     // MovewidgetInstance의 이동 시작 함수 호출
//     if (MovewidgetInstance)
//     {
//         MovewidgetInstance->OnStartButtonClicked();
//         UE_LOG(LogTemp, Warning, TEXT("Movement started from OnStartButtonClicked in Movewidget"));
//     }
//     else
//     {
//         UE_LOG(LogTemp, Error, TEXT("MovewidgetInstance is null, cannot start movement"));
//     }
// 
//     // RotationWidgetInstance의 회전 시작 함수 호출
//     if (RotationWidgetInstance)
//     {
//         RotationWidgetInstance->OnRotateStartClicked();
//         UE_LOG(LogTemp, Warning, TEXT("Rotation started from OnRotateStartClicked in RotationWidget"));
//     }
//     else
//     {
//         UE_LOG(LogTemp, Error, TEXT("RotationWidgetInstance is null, cannot start rotation"));
//     }
}



// 충돌 끝 시 원래 스케일 복원
void AMyStartButton::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor->IsA(APSH_Player::StaticClass()))
    {
        ButtonMesh->SetWorldScale3D(OriginalScale);
    }
}


