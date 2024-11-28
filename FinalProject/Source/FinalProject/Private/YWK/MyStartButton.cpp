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

    RootComponent = meshComp;

//     RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
//     RootComponent = RootSceneComponent;

    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
    BaseMesh->SetupAttachment(RootComponent);

//     ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
//     RootComponent = ButtonMesh;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    CollisionBox->SetupAttachment(RootComponent);
    CollisionBox->SetCollisionProfileName(TEXT("Trigger"));
    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AMyStartButton::OnOverlapBegin);
    CollisionBox->OnComponentEndOverlap.AddDynamic(this, &AMyStartButton::OnOverlapEnd);

    OriginalScale = FVector(0.7f, 0.7f, 0.2f);
    PressedScale = FVector(OriginalScale.X, OriginalScale.Y, 0.05f);

    bReplicates = true;
    SetReplicateMovement(true);
}

void AMyStartButton::BeginPlay()
{
    Super::BeginPlay();
    
    mapBlock = true;
    
    OriginalScale = GetActorScale3D();

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
    if (bIsOverlapping) return; // 재진입 방지
    bIsOverlapping = true;

    if (Cast<APSH_Player>(OtherActor))
    {
        FVector CurrentScale = GetActorScale3D();
      
        FVector NewScale = FVector(OriginalScale.X, OriginalScale.Y, 0.05f);
       
       SetActorScale3D(NewScale);
        
        if (GetOwner() != Cast<APSH_Player>(OtherActor))
        {
            SetOwner(Cast<APSH_Player>(OtherActor));
        }

        if (HasAuthority() && !bIsOverlapping)
        {
            APSH_GameModeBase* GM = Cast<APSH_GameModeBase>(GetWorld()->GetAuthGameMode());

            if (GM)
            {
                bmodeChange = !bmodeChange;
                GM->SetCreateingCheck(bmodeChange);
                PRINTLOG(TEXT("SetCreateingCheck"));
            }
        }
    }
}

// 충돌 끝 시 원래 스케일 복원
void AMyStartButton::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && Cast<APSH_Player>(OtherActor))
    {
        SetActorScale3D(OriginalScale);
        bIsOverlapping = false; // 재진입 플래그 해제
    }
}


