#include "YWK/RotationWidget.h"
#include "YWK/MyRotateActorComponent.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/EditableText.h"
#include "Components/ComboBoxString.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

void URotationWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (RotateSpeedText)
    {
        UE_LOG(LogTemp, Warning, TEXT("RotateSpeedText initialized successfully in RotationWidget"));
        RotateSpeedText->OnTextCommitted.AddDynamic(this, &URotationWidget::OnRotateSpeedTextCommitted);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("RotateSpeedText is null in RotationWidget"));
    }

    if (Btn_X)
    {
        Btn_X->OnClicked.AddDynamic(this, &URotationWidget::OnXClicked);
    }
    if (Btn_Y)
    {
        Btn_Y->OnClicked.AddDynamic(this, &URotationWidget::OnYClicked);
    }
    if (Btn_Z)
    {
        Btn_Z->OnClicked.AddDynamic(this, &URotationWidget::OnZClicked);
    }
    if (Btn_RotateBack)
    {
        Btn_RotateBack->OnClicked.AddDynamic(this, &URotationWidget::OnRotateBackClicked);
    }
    if (Btn_RotateStart)
    {
        Btn_RotateStart->OnClicked.AddDynamic(this, &URotationWidget::OnRotateStartClicked);
    }
    if (Btn_RotateStop)
    {
        Btn_RotateStop->OnClicked.AddDynamic(this, &URotationWidget::OnRotateStopClicked);
    }
    if (Btn_RotateOrigin)
    {
        Btn_RotateOrigin->OnClicked.AddDynamic(this, &URotationWidget::OnRotateOriginClicked);
    }

    if (TimesRotateText)
    {
        TimesRotateText->OnTextCommitted.AddDynamic(this, &URotationWidget::UpdateTimesTextUI);
    }
    if (RotateLoop)
    {
        RotateLoop->OnCheckStateChanged.AddDynamic(this, &URotationWidget::OnLoopModeCheckChanged);
    }

    // 콤보박스 글자 색 바꾸기
    if (RotateBoxList)
    {
        RotateBoxList->OnGenerateWidgetEvent.BindDynamic(this, &URotationWidget::GenerateComboBoxItem);
    }

    InitializeFunctionObjects();
}

void URotationWidget::OnXClicked()
{
    if (SelectedActor)
    {
        SetRotationDirection(FRotator(1.0f, 0.0f, 0.0f));
    }
}

void URotationWidget::OnYClicked()
{
    if (SelectedActor)
    {
        SetRotationDirection(FRotator(0.0f, 1.0f, 0.0f));
    }
}

void URotationWidget::OnZClicked()
{
    if (SelectedActor)
    {
        SetRotationDirection(FRotator(0.0f, 0.0f, 1.0f));
    }
}

void URotationWidget::OnRotateBackClicked()
{
    RemoveFromParent();
    if (ActionChoice)
    {
        UUserWidget* BackMoveWidget = CreateWidget<UUserWidget>(GetWorld(), ActionChoice);
        if (BackMoveWidget)
        {
            BackMoveWidget->AddToViewport();
        }
    }
}

void URotationWidget::SetRotationDirection(FRotator NewDirection)
{
    if (SelectedActor)
    {
        if (UMyRotateActorComponent* SelectedRotateComponent = SelectedActor->FindComponentByClass<UMyRotateActorComponent>())
        {
            RotateDirectionMap.FindOrAdd(SelectedRotateComponent) = NewDirection;
            UE_LOG(LogTemp, Warning, TEXT("SetRotateDirection called with direction: %s"), *NewDirection.ToString());
        }
    }
}

void URotationWidget::OnRotateStartClicked()
{
    if (ControlledRotateComponents.Num() > 0)
    {
        for (UMyRotateActorComponent* RotateComponent : ControlledRotateComponents)
        {
            if (RotateComponent)
            {
                RotateComponent->RotateDirection = RotateDirectionMap.Contains(RotateComponent) ? RotateDirectionMap[RotateComponent] : FRotator(1.0f, 0.0f, 0.0f);
                RotateComponent->RotateSpeed = RotateSpeedMap.Contains(RotateComponent) ? RotateSpeedMap[RotateComponent] : 100.0f;

                UE_LOG(LogTemp, Warning, TEXT("Starting rotation for %s with speed: %f and direction: %s"),
                    *RotateComponent->GetOwner()->GetName(),
                    RotateComponent->RotateSpeed,
                    *RotateComponent->RotateDirection.ToString());
                RotateComponent->StartRolling();
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("No direction or speed set for %s; rotation skipped"), *RotateComponent->GetOwner()->GetName());
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No ControlledRotateComponents found."));
    }
}

void URotationWidget::OnRotateStopClicked()
{
    if (SelectedActor)
    {
        if (UMyRotateActorComponent* SelectedRotateComponent = SelectedActor->FindComponentByClass<UMyRotateActorComponent>())
        {
            SelectedRotateComponent->StopRolling();
            UE_LOG(LogTemp, Warning, TEXT("Stopped rolling for component %s"), *SelectedRotateComponent->GetOwner()->GetName());
        }
    }
}

void URotationWidget::OnRotateOriginClicked()
{
    for (UMyRotateActorComponent* RotateComponent : ControlledRotateComponents)
    {
        if (RotateComponent)
        {
            RotateComponent->OriginRolling();
        }
    }
}

void URotationWidget::OnRotateSpeedTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    if (CommitMethod == ETextCommit::OnEnter && SelectedActor)
    {
        float SpeedValue = FCString::Atof(*Text.ToString());
        UE_LOG(LogTemp, Warning, TEXT("SpeedValue from EditableTextBox: %f"), SpeedValue);

        if (UMyRotateActorComponent* SelectedRotateComponent = SelectedActor->FindComponentByClass<UMyRotateActorComponent>())
        {
            RotateSpeedMap.FindOrAdd(SelectedRotateComponent) = SpeedValue;
            UE_LOG(LogTemp, Warning, TEXT("Rotation speed set to: %f for component %s"), SpeedValue, *SelectedRotateComponent->GetOwner()->GetName());
            
            UpdateRotateSpeedMap();  // RotateSpeedMap 갱신 시 호출
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("SelectedRotateComponent is null, cannot set speed"));
        }
    }
}

void URotationWidget::OnLoopModeCheckChanged(bool bIsChecked)
{
    if (SelectedActor)
    {
        if (UMyRotateActorComponent* SelectedRotateComponent = SelectedActor->FindComponentByClass<UMyRotateActorComponent>())
        {
            SelectedRotateComponent->bLoopMode = bIsChecked;
            UE_LOG(LogTemp, Warning, TEXT("Loop mode set to: %s for component %s"), bIsChecked ? TEXT("Enabled") : TEXT("Disabled"), *SelectedRotateComponent->GetOwner()->GetName());
        }
    }
}

void URotationWidget::InitializeFunctionObjects()
{
    if (ControlledRotateComponents.Num() > 0)
    {
        return;
    }

    FSoftClassPath BP_FunctionObjectClassRef(TEXT("/Game/YWK/BP/BP_Rotate.BP_Rotate_C"));
    UClass* BP_FunctionObjectClass = BP_FunctionObjectClassRef.TryLoadClass<AActor>();

    if (BP_FunctionObjectClass && GetWorld())
    {
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), BP_FunctionObjectClass, AllFunctionObject);

        if (RotateBoxList)
        {
            for (AActor* FunctionObject : AllFunctionObject)
            {
                if (FunctionObject)
                {
                    RotateBoxList->AddOption(FunctionObject->GetName());
                    if (UMyRotateActorComponent* RotateComponent = FunctionObject->FindComponentByClass<UMyRotateActorComponent>())
                    {
                        AddControlledRotateComponent(RotateComponent);
                    }
                }
            }

            // OnSelectionChanged 이벤트 바인딩
            RotateBoxList->OnSelectionChanged.AddDynamic(this, &URotationWidget::OnFunctionObjectSelected);

            // OnGenerateWidgetEvent 바인딩
            RotateBoxList->OnGenerateWidgetEvent.BindDynamic(this, &URotationWidget::GenerateComboBoxItem);
        }
        if (AllFunctionObject.Num() > 0)
        {
            SelectedActor = AllFunctionObject[0];
        }
    }
}


void URotationWidget::AddObjectToComboBox(AActor* NewObject)
{
    if (NewObject && RotateBoxList)
    {
        FString DisplayName = NewObject->GetName();
        RotateBoxList->AddOption(DisplayName);
        AllFunctionObject.Add(NewObject);
        UE_LOG(LogTemp, Warning, TEXT("Added new object to ComboBox: %s"), *DisplayName);
    }
}

void URotationWidget::OnFunctionObjectSelected(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    int32 SelectedIndex = RotateBoxList->FindOptionIndex(SelectedItem);
    if (SelectedIndex != INDEX_NONE && AllFunctionObject.IsValidIndex(SelectedIndex))
    {
        SelectedActor = AllFunctionObject[SelectedIndex];
        UMyRotateActorComponent* RotateComponent = SelectedActor->FindComponentByClass<UMyRotateActorComponent>();

        if (RotateComponent)
        {
            if (RotateSpeedText)
            {
                float SpeedValue = RotateSpeedMap.Contains(RotateComponent) ? RotateSpeedMap[RotateComponent] : 0.f;
                RotateSpeedText->SetText(FText::AsNumber(SpeedValue));
            }

            if (TimesRotateText)
            {
                int32 TimesValue = TimesRotateMap.Contains(RotateComponent) ? TimesRotateMap[RotateComponent] : 0;
                TimesRotateText->SetText(FText::AsNumber(TimesValue));
            }

            if (RotateLoop)
            {
                bool bLoop = RotateComponent->bLoopMode;
                RotateLoop->SetIsChecked(bLoop);
            }
        }
    }
}

void URotationWidget::AddControlledRotateComponent(UMyRotateActorComponent* NewComponent)
{
    if (NewComponent && !ControlledRotateComponents.Contains(NewComponent))
    {
        ControlledRotateComponents.Add(NewComponent);
        UE_LOG(LogTemp, Warning, TEXT("Added RotateComponent of %s to ControlledRotateComponents"), *NewComponent->GetOwner()->GetName());
    }
}

void URotationWidget::UpdateTimesTextUI(const FText& Text, ETextCommit::Type CommitMethod)
{
    if (CommitMethod == ETextCommit::OnEnter)
    {
        int32 TimesValue = FCString::Atoi(*Text.ToString());
        for (UMyRotateActorComponent* RotateComponent : ControlledRotateComponents)
        {
            if (RotateComponent)
            {
                TimesRotateMap.FindOrAdd(RotateComponent) = TimesValue;
                UE_LOG(LogTemp, Warning, TEXT("MaxRotate set to: %d for component %s"), TimesValue, *RotateComponent->GetOwner()->GetName());
            }
        }
    }
}

void URotationWidget::UpdateComponentSettings(UMyRotateActorComponent* RotateComponent)
{
    if (!RotateComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("UpdateComponentSettings called with null RotateComponent."));
        return;
    }

    if (!RotateSpeedText || !TimesRotateText)
    {
        UE_LOG(LogTemp, Warning, TEXT("UpdateComponentSettings called with null RotateSpeedText or TimesRotateText."));
        return;
    }

    float SpeedValue = FCString::Atof(*RotateSpeedText->GetText().ToString());
    int32 TimesValue = FCString::Atoi(*TimesRotateText->GetText().ToString());

    RotateComponent->RotateSpeed = SpeedValue;
    RotateComponent->MaxRotate = TimesValue;

    UE_LOG(LogTemp, Warning, TEXT("Updated component settings: Speed=%f, MaxRotate=%d for component %s"), SpeedValue, TimesValue, *RotateComponent->GetOwner()->GetName());
}

UWidget* URotationWidget::GenerateComboBoxItem(FString Item)
{
    UTextBlock* TextBlock = NewObject<UTextBlock>(this);  // TextBlock 생성
    if (TextBlock)
    {
        TextBlock->SetText(FText::FromString(Item));  // 텍스트 설정
        TextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Green));  // 텍스트 색상
        TextBlock->Font.Size = 16;  // 텍스트 크기 설정
    }
    return TextBlock;  // UWidget 반환
}

void URotationWidget::StartRotation()
{
    UE_LOG(LogTemp, Warning, TEXT("Starting rotation for all controlled components."));

    for (auto& Entry : RotateDirectionMap)
    {
        UE_LOG(LogTemp, Warning, TEXT("DirectionMap Entry - Component: %s, Direction: %s"), *Entry.Key->GetOwner()->GetName(), *Entry.Value.ToString());
    }

    for (auto& Entry : RotateSpeedMap)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpeedMap Entry - Component: %s, Speed: %f"), *Entry.Key->GetOwner()->GetName(), Entry.Value);
    }

    for (UMyRotateActorComponent* RotateComponent : ControlledRotateComponents)
    {
        if (RotateComponent && RotateDirectionMap.Contains(RotateComponent) && RotateSpeedMap.Contains(RotateComponent))
        {
            RotateComponent->RotateDirection = RotateDirectionMap[RotateComponent];
            RotateComponent->RotateSpeed = RotateSpeedMap[RotateComponent];

            UE_LOG(LogTemp, Warning, TEXT("Starting rotation for %s with speed: %f and direction: %s"),
                *RotateComponent->GetOwner()->GetName(),
                RotateComponent->RotateSpeed,
                *RotateComponent->RotateDirection.ToString());

            RotateComponent->StartRolling();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("No direction or speed set for %s; rotation skipped"), *RotateComponent->GetOwner()->GetName());
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Rotation started for all components."));
}

void URotationWidget::RetryInitialization()
{
    if (RotateSpeedText)
    {
        UE_LOG(LogTemp, Warning, TEXT("RotateSpeedText initialized on delayed attempt"));
        GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Retrying RotateSpeedText initialization..."));
    }
}

AActor* URotationWidget::GetOwnerFromComponent()
{
    if (SelectedActor)
    {
        return SelectedActor;
    }

    FSoftClassPath BP_FunctionObjectClassRef(TEXT("/Game/YWK/BP/BP_Rotate.BP_Rotate_C"));
    UClass* BP_FunctionObjectClass = BP_FunctionObjectClassRef.TryLoadClass<AActor>();

    if (BP_FunctionObjectClass)
    {
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), BP_FunctionObjectClass, FoundActors);

        if (FoundActors.Num() > 0)
        {
            SelectedActor = FoundActors[0];
            return SelectedActor;
        }
        else
        {
            SelectedActor = GetWorld()->SpawnActor<AActor>(BP_FunctionObjectClass);
            return SelectedActor;
        }
    }
    return nullptr;
}

void URotationWidget::UpdateRotateSpeedMap()
{
    OnRotateSpeedMapUpdated.Broadcast();
    UE_LOG(LogTemp, Warning, TEXT("RotateSpeedMap updated and event broadcasted."));
}
