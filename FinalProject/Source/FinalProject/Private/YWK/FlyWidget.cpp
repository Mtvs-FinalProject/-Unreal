// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/FlyWidget.h"
#include "Components/Button.h"
#include "YWK/ActionChoice.h"
#include "Components/EditableText.h"
#include "Kismet/GameplayStatics.h"
#include "YWK/MyFlyActorComponent.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"

void UFlyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_UP)
	{
		Btn_UP->OnClicked.AddDynamic(this, &UFlyWidget::OnUpButtonClicked);
	}
	if (Btn_Down)
	{
		Btn_Down->OnClicked.AddDynamic(this, &UFlyWidget::OnDownButtonClicked);
	}
	if (Btn_Back)
	{
		Btn_Back->OnClicked.AddDynamic(this, &UFlyWidget::OnBackButtonClicked);
	}

	if (Btn_Start)
	{
		Btn_Start->OnClicked.AddDynamic(this, &UFlyWidget::OnStartButtonClicked);
	}

	if (Btn_Stop)
	{
		Btn_Stop->OnClicked.AddDynamic(this, &UFlyWidget::OnStopButtonClicked);
	}

	if (Btn_Origin)
	{
		Btn_Origin->OnClicked.AddDynamic(this, &UFlyWidget::OnOriginButtonClicked);
	}
	
	// �ӵ� �Է� �ʵ忡 OnTextCommitted �̺�Ʈ ���ε�
	if (FlySpeedText)
	{
		FlySpeedText->OnTextCommitted.AddDynamic(this, &UFlyWidget::OnFlySpeedTextCommitted);
	}

	// �Ÿ� �Է� �ʵ忡 OnTextCommitted �̺�Ʈ ���ε�
	if (FlyHightText)
	{
		FlyHightText->OnTextCommitted.AddDynamic(this, &UFlyWidget::OnFlyDistanceTextCommitted);
	}
	// Initialize function objects in ComboBox
	InitializeFunctionObjects();

	// �պ���� üũ �ڽ�
	if (Chk_LoopMode)
	{
		Chk_LoopMode->OnCheckStateChanged.AddDynamic(this, &UFlyWidget::OnLoopModeCheckChanged);
	}

	// �Ϲ��� üũ �ڽ�
	if (Chk_SingleDirectionMode)
	{
		Chk_SingleDirectionMode->OnCheckStateChanged.AddDynamic(this, &UFlyWidget::OnSingleDirectionCheckChanged);
	}

	// �պ� �ؽ�Ʈ �ڽ�
	if (Txt_LoopCount)
	{
		Txt_LoopCount->OnTextCommitted.AddDynamic(this, &UFlyWidget::OnLoopCountCommitted);
	}
}

// ���� �ö󰡱� ��ư
void UFlyWidget::OnUpButtonClicked()
{
	StoredFlyDirection = FVector(0.0f, 0.0f, 1.0f);
	UpdatePreviewLocation(StoredFlyDirection, FCString::Atof(*FlyHightText->GetText().ToString()));
	UE_LOG(LogTemp, Warning, TEXT("Fly direction set to Up."));
}

// �Ʒ��� �������� ��ư
void UFlyWidget::OnDownButtonClicked()
{
	StoredFlyDirection = FVector(0.0f, 0.0f, -1.0f);
	UpdatePreviewLocation(StoredFlyDirection, FCString::Atof(*FlyHightText->GetText().ToString()));
	UE_LOG(LogTemp, Warning, TEXT("Fly direction set to Down."));
}

// ���� ����
void UFlyWidget::OnStartButtonClicked()
{
	if (PreviewActor)
	{
		PreviewActor->SetActorHiddenInGame(true);
		PreviewActor = nullptr;
	}

	if (SelectedActor)
	{
		UMyFlyActorComponent* FlyComponent = SelectedActor->FindComponentByClass<UMyFlyActorComponent>();
		if (FlyComponent)
		{
			FlyComponent->FlyDirection = StoredFlyDirection.IsNearlyZero() ? FVector(0.0f, 0.0f, 1.0f) : StoredFlyDirection;
			FlyComponent->StartFly();
			UE_LOG(LogTemp, Warning, TEXT("Started flight for %s with direction: %s"), *SelectedActor->GetName(), *FlyComponent->FlyDirection.ToString());
		}
	}

	for (UMyFlyActorComponent* FlyComponent : ControlledFlyComponents)
	{
		if (FlyComponent)
		{
			FlyComponent->FlyDirection = StoredFlyDirection.IsNearlyZero() ? FVector(0.0f, 0.0f, 1.0f) : StoredFlyDirection;
			FlyComponent->StartFly();
			UE_LOG(LogTemp, Warning, TEXT("Started flight for component: %s with direction: %s"), *FlyComponent->GetOwner()->GetName(), *FlyComponent->FlyDirection.ToString());
		}
	}
}

// ���� ����
void UFlyWidget::OnStopButtonClicked()
{
	if (SelectedActor)
	{
		UMyFlyActorComponent* FlyComponent = SelectedActor->FindComponentByClass<UMyFlyActorComponent>();
		if (FlyComponent)
		{
			FlyComponent->StopFly();
		}
	}

	for (UMyFlyActorComponent* FlyComponent : ControlledFlyComponents)
	{
		if (FlyComponent)
		{
			FlyComponent->StopFly();
		}
	}
}

// ���� ��ġ�� ���ư���
void UFlyWidget::OnOriginButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Origin button clicked"));

	// ������Ʈ ���� ��������
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyFlyActorComponent* FlyComponent = Owner->FindComponentByClass<UMyFlyActorComponent>();
		if (FlyComponent)
		{
			FlyComponent->OriginFly();
			UE_LOG(LogTemp, Warning, TEXT("Origin button clicked: calling OriginFly()"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("FlyComponent not found"));
		}
	}
}

// �ڷΰ��� ��ư
void UFlyWidget::OnBackButtonClicked()
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

// �ӵ��� ���� �� set
void UFlyWidget::UpdateMovementValuesInUI(float SpeedValue = -1.0f, float DistanceValue = -1.0f)
{
	// �ӵ� ���� UI�� EditableText�� ����
	if (FlySpeedText)
	{
		// SpeedValue�� ����(-1.0f)�� ��� ����ΰ�, �׷��� ������ �� ����
		FlySpeedText->SetText(SpeedValue < 0.0f ? FText::GetEmpty() : FText::AsNumber(SpeedValue));
	}
	// �Ÿ� ���� UI�� EditableText�� ����
	if (FlyHightText)
	{
		// DistanceValue�� ����(-1.0f)�� ��� ����ΰ�, �׷��� ������ �� ����
		FlyHightText->SetText(DistanceValue < 0.0f ? FText::GetEmpty() : FText::AsNumber(DistanceValue));
	}
}


// �ӵ��� �Ÿ� �� �ִ� �Լ�
void UFlyWidget::ApplyMovementValues()
{
	// ������Ʈ ��������
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyFlyActorComponent* FlyComponent = Owner->FindComponentByClass<UMyFlyActorComponent>();
		if (FlyComponent)
		{
			// �ӵ��� ����
			FString SpeedString = FlySpeedText->GetText().ToString();
			float SpeedValue = FCString::Atof(*SpeedString); // ���ڿ� float�� �ٲٱ�
			FlyComponent->FlySpeed = SpeedValue;

			// �Ÿ��� ����
			FString DistnaceString = FlyHightText->GetText().ToString();
			float DistanceValue = FCString::Atof(*DistnaceString); // �̰͵� float���� �ٲٱ�
			FlyComponent->FlyDistance = DistanceValue;

			// UI�� �� ������Ʈ
			UpdateMovementValuesInUI(FlyComponent->FlySpeed, FlyComponent->FlyDistance);
		}
	}
}

// �ӵ� �Է� �ʵ忡�� ���͸� ���� ��
void UFlyWidget::OnFlySpeedTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		// �Էµ� �ӵ� ���� ó��
		FString SpeedString = Text.ToString();
		float SpeedValue = FCString::Atof(*SpeedString);

		if (AActor* Owner = GetOwnerFromComponent())
		{
			UMyFlyActorComponent* FlyComponent = Owner->FindComponentByClass<UMyFlyActorComponent>();
			if (FlyComponent)
			{
				FlyComponent->FlySpeed = SpeedValue;
				// UI�� �� ������Ʈ
				UE_LOG(LogTemp, Warning, TEXT("Speed set to: %f"), SpeedValue);
			}
		}
	}
}

// ���� �Է� �ʵ忡�� ���͸� ���� ��
void UFlyWidget::OnFlyDistanceTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		// �Էµ� �Ÿ� ���� ó��
		float DistValues = FCString::Atof(*Text.ToString());

		if (AActor* Owner = GetOwnerFromComponent())
		{
			UMyFlyActorComponent* FlyComponent = Owner->FindComponentByClass<UMyFlyActorComponent>();
			if (FlyComponent)
			{
				FlyComponent->MaxFlyDistance = DistValues;
				UE_LOG(LogTemp, Warning, TEXT("Distance set to: %f"), DistValues);
				UpdatePreviewLocation(StoredFlyDirection, DistValues); // �̸����� ��ġ ������Ʈ �߰�
			}
		}
	}
}

AActor* UFlyWidget::GetOwnerFromComponent()
{
	if (SelectedActor)
	{
		return SelectedActor; // ������ ���Ͱ� ������ �ٷ� ��ȯ
	}

	// ������ ���Ͱ� ���� ���, BP_FunctionObject Ŭ�������� ã��
	FStringClassReference BP_FunctionObjectClassRef(TEXT("/Game/YWK/BP/BP_MoveandFly.BP_MoveandFly_C"));
	UClass* BP_FunctionObjectClass = BP_FunctionObjectClassRef.TryLoadClass<AActor>();

	if (BP_FunctionObjectClass)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), BP_FunctionObjectClass, FoundActors);

		if (FoundActors.Num() > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found BP_FunctionObject: %s"), *FoundActors[0]->GetName());
			SelectedActor = FoundActors[0]; // ù ��° ���͸� ���õ� ���ͷ� ����
			return SelectedActor;
		}

		// ������ ���� ����
		SelectedActor = GetWorld()->SpawnActor<AActor>(BP_FunctionObjectClass);
		return SelectedActor;
	}
	return nullptr;
}


void UFlyWidget::InitializeFunctionObjects()
{
	FStringClassReference BP_FunctionObjectClassRef(TEXT("/Game/YWK/BP/BP_MoveandFly.BP_MoveandFly_C"));
	UClass* BP_FunctionObjectClass = BP_FunctionObjectClassRef.TryLoadClass<AActor>();

	if (BP_FunctionObjectClass && GetWorld())
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), BP_FunctionObjectClass, AllFunctionObject);

		if (FlyBoxList)
		{
			for (AActor* FunctionObject : AllFunctionObject)
			{
				if (FunctionObject)
				{
					FlyBoxList->AddOption(FunctionObject->GetName());
				}
			}
			FlyBoxList->OnSelectionChanged.AddDynamic(this, &UFlyWidget::OnFunctionObjectSelected);
		}

		if (AllFunctionObject.Num() > 0)
		{
			SelectedActor = AllFunctionObject[0];
		}
	}
}

void UFlyWidget::AddObjectToComboBox(AActor* NewObject)
{
	if (NewObject && FlyBoxList)
	{
		//Display name�� �߰��� �� Object�� Label�� ���
		FString DisplayName = NewObject->GetActorLabel();
		FlyBoxList->AddOption(DisplayName);

		//AllFunctionObject �迭�� �߰��Ͽ� ���߿� ���� �����ϰ� 
		AllFunctionObject.Add(NewObject);
		UE_LOG(LogTemp, Warning, TEXT("Added new object to ComboBox: %s"), *DisplayName);
	}
}

// ������ ������Ʈ�� ���� ���� FlyComponent�� ����
void UFlyWidget::OnFunctionObjectSelected(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	int32 SelectedIndex = FlyBoxList->FindOptionIndex(SelectedItem);
	if (SelectedIndex != INDEX_NONE && AllFunctionObject.IsValidIndex(SelectedIndex))
	{
		SelectedActor = AllFunctionObject[SelectedIndex];
		UE_LOG(LogTemp, Warning, TEXT("Selected fly function object: %s"), *SelectedActor->GetName());

		// ���õ� ������Ʈ�� FlyComponent ����
		if (UMyFlyActorComponent* FlyComponent = SelectedActor->FindComponentByClass<UMyFlyActorComponent>())
		{
			// UI ��ҿ� FlyComponent �� ����ȭ ��� �ʱ� ���·� ���
			UpdateMovementValuesInUI(); // �Ķ���� �������� �������ν� �⺻�� ����
			Chk_LoopMode->SetIsChecked(FlyComponent->bLoopMode);
			Chk_SingleDirectionMode->SetIsChecked(FlyComponent->bSingleDirection);

			// �պ� Ƚ�� ������Ʈ
			if (Txt_LoopCount)
			{
				Txt_LoopCount->SetText(FText::AsNumber(FlyComponent->LoopCount));
			}
		}
	}
}

// �պ� ��� üũ�ڽ� ���� ����
void UFlyWidget::OnLoopModeCheckChanged(bool bIsChecked)
{
	if (SelectedActor)
	{
		UMyFlyActorComponent* FlyComponent = SelectedActor->FindComponentByClass<UMyFlyActorComponent>();
		if (FlyComponent)
		{
			FlyComponent->bLoopMode = bIsChecked;
			if (bIsChecked && Chk_SingleDirectionMode)
			{
				Chk_SingleDirectionMode->SetIsChecked(false);
			}
			UE_LOG(LogTemp, Warning, TEXT("Loop mode set to: %s for %s"), bIsChecked ? TEXT("Enabled") : TEXT("Disabled"), *SelectedActor->GetName());
		}
	}
}

// �ܼ� �̵� ��� üũ�ڽ� ���� ����
void UFlyWidget::OnSingleDirectionCheckChanged(bool bIsChecked)
{
	if (SelectedActor)
	{
		UMyFlyActorComponent* FlyComponent = SelectedActor->FindComponentByClass<UMyFlyActorComponent>();
		if (FlyComponent)
		{
			FlyComponent->bSingleDirection = bIsChecked;
			if (bIsChecked && Chk_LoopMode)
			{
				Chk_LoopMode->SetIsChecked(false);
			}
			UE_LOG(LogTemp, Warning, TEXT("Single Direction mode set to: %s for %s"), bIsChecked ? TEXT("Enabled") : TEXT("Disabled"), *SelectedActor->GetName());
		}
	}
}

// �պ� Ƚ�� �Է� �ʵ忡�� ���͸� ���� ��
void UFlyWidget::OnLoopCountCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter && SelectedActor)
	{
		if (UMyFlyActorComponent* FlyComponent = SelectedActor->FindComponentByClass<UMyFlyActorComponent>())
		{
			int32 LoopValue = FCString::Atoi(*Text.ToString());
			FlyComponent->LoopCount = LoopValue;
			FlyComponent->CurrentLoop = 0;  // ���ο� ���� ���� �� ���� �ʱ�ȭ
			UE_LOG(LogTemp, Warning, TEXT("Loop count set to: %d for %s"), LoopValue, *SelectedActor->GetName());
		}
	}
}

// ���� FlyComponent �߰� �Լ�
void UFlyWidget::AddControlledFlyComponent(UMyFlyActorComponent* NewFlyComponent)
{
	if (NewFlyComponent && !ControlledFlyComponents.Contains(NewFlyComponent))
	{
		ControlledFlyComponents.Add(NewFlyComponent);
	}
}

// ������ ���� ���� �Լ�
void UFlyWidget::SpawnPreviewActor()
{
	if (!PreviewActor)
	{
		FStringClassReference PreviewActorClassRef(TEXT("/Game/YWK/BP/BP_PreviewDistance.BP_PreviewDistance_C"));
		UClass* PreviewActorClass = PreviewActorClassRef.TryLoadClass<AActor>();

		if (PreviewActorClass)
		{
			PreviewActor = GetWorld()->SpawnActor<AActor>(PreviewActorClass, FVector::ZeroVector, FRotator::ZeroRotator);
			if (PreviewActor)
			{
				PreviewActor->SetActorHiddenInGame(false);
			}
		}
	}
}

// ������ ��ġ ������Ʈ �Լ�
void UFlyWidget::UpdatePreviewLocation(FVector Direction, float Distance)
{
	if (PreviewActor)
	{
		PreviewActor->Destroy();
		PreviewActor = nullptr;
	}

	if (SelectedActor)
	{
		FVector TargetLocation = SelectedActor->GetActorLocation() + (Direction * Distance);
		SpawnPreviewActor();
		if (PreviewActor)
		{
			PreviewActor->SetActorLocation(TargetLocation);
		}
	}
}

