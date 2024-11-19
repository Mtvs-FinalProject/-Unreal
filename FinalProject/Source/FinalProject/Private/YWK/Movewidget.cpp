// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/Movewidget.h"
#include "Components/Button.h"
#include "YWK/ActionChoice.h"
#include "YWK/MyMoveActorComponent.h"
#include "PSH/PSH_GameMode/PSH_GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/EditableText.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"

void UMovewidget::NativeConstruct()
{
	Super::NativeConstruct();

	// ���Ӹ�忡�� ������Ʈ ���� �̺�Ʈ�� ���ε�
	APSH_GameModeBase* GameMode = Cast<APSH_GameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->OnObjectCreated.AddDynamic(this, &UMovewidget::AddObjectToComboBox);
	}

	// �ڷΰ��� ��ư
	if (Btn_MoveBack)
	{
		Btn_MoveBack->OnClicked.AddDynamic(this, &UMovewidget::OnMoveBackClicked);
	}
	// ������ ���� ��ư
	if (Btn_Front)
	{
		Btn_Front->OnClicked.AddDynamic(this, &UMovewidget::OnFrontClicked);
	}

	// �ڷ� ���� ��ư
	if (Btn_Back)
	{
		Btn_Back->OnClicked.AddDynamic(this, &UMovewidget::OnBackClicked);
	}

	// �������� ���� ��ư
	if (Btn_Left)
	{
		Btn_Left->OnClicked.AddDynamic(this, &UMovewidget::OnLeftClicked);
	}

	// ���������� ���� ��ư
	if (Btn_Right)
	{
		Btn_Right->OnClicked.AddDynamic(this, &UMovewidget::OnRightClicked);
	}

	// ��� ���� ��ư
	if (Btn_MoveStart)
	{
		Btn_MoveStart->OnClicked.AddDynamic(this, &UMovewidget::OnStartButtonClicked);
	}

	// ��� ���� ��ư
	if (Btn_MoveStop)
	{
		Btn_MoveStop->OnClicked.AddDynamic(this, &UMovewidget::OnStopButtonClicked);
	}

	// ���� �ڸ��� ���ư���
	if (Btn_MoveOrigin)
	{
		Btn_MoveOrigin->OnClicked.AddDynamic(this, &UMovewidget::OnOriginButtonClicked);
	}

	// �ӵ� �Է� �ʵ忡 OnTextCommitted �̺�Ʈ ���ε�
	if (SpeedMoveText)
	{
		SpeedMoveText->OnTextCommitted.AddDynamic(this, &UMovewidget::OnSpeedTextCommitted);
	}

	// �Ÿ� �Է� �ʵ忡 OnTextCommitted �̺�Ʈ ���ε�
	if (DistMoveText)
	{
		DistMoveText->OnTextCommitted.AddDynamic(this, &UMovewidget::OnDistanceTextCommitted);
	}

	InitializeFunctionObjects();

	// �պ� ��� üũ�ڽ��� Ƚ�� �Է� �ʵ� �̺�Ʈ ���ε�
	if (Chk_LoopMode)
	{
		Chk_LoopMode->OnCheckStateChanged.AddDynamic(this, &UMovewidget::OnLoopModeCheckChanged);
	}

	if (Chk_SingleDirectionMode)
	{
		Chk_SingleDirectionMode->OnCheckStateChanged.AddDynamic(this, &UMovewidget::OnSingleDirectionCheckChanged);
	}
}

// ������ ����
void UMovewidget::OnFrontClicked()
{
	SetMoveDirection(FVector(1.0f, 0.0f, 0.0f));  // ���� ����
}

// �ڷ� ����
void UMovewidget::OnBackClicked()
{
	SetMoveDirection(FVector(-1.0f, 0.0f, 0.0f));  // �Ĺ� ����
}

// ���������� ����
void UMovewidget::OnRightClicked()
{
	SetMoveDirection(FVector(0.0f, 1.0f, 0.0f));  // ������ ����
}

// �������� ����
void UMovewidget::OnLeftClicked()
{
	SetMoveDirection(FVector(0.0f, -1.0f, 0.0f));  // ���� ����
}

// ���� ��� ui ����� �ڿ� ui ����
void UMovewidget::OnMoveBackClicked()
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

// ��� ���� ��ư
void UMovewidget::OnStartButtonClicked()
{
	// Start ��ư�� ���� �� �̸����� ������Ʈ�� ��������� ����
	if (PreviewActor)
	{
		PreviewActor->SetActorHiddenInGame(true);
		PreviewActor = nullptr;
	}
	// ���� SelectedActor ó��
	if (SelectedActor)
	{
		UMyMoveActorComponent* MoveComponent = SelectedActor->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			MoveComponent->MoveDirection = MoveDirectionMap.Contains(MoveComponent) ? MoveDirectionMap[MoveComponent] : StoredMoveDirection;
			MoveComponent->MoveSpeed = MoveSpeedMap.Contains(MoveComponent) ? MoveSpeedMap[MoveComponent] : MoveComponent->MoveSpeed;
			MoveComponent->StartMoving();
			UE_LOG(LogTemp, Warning, TEXT("Starting movement for %s with direction: %s and speed: %f"),
				*SelectedActor->GetName(),
				*MoveComponent->MoveDirection.ToString(),
				MoveComponent->MoveSpeed);
		}
	}

	// ���� ������Ʈ ó��
	for (UMyMoveActorComponent* MoveComponent : ControlledMoveComponents)
	{
		if (MoveComponent)
		{
			FVector Direction = MoveDirectionMap.Contains(MoveComponent) ? MoveDirectionMap[MoveComponent] : FVector(1.0f, 0.0f, 0.0f);
			float Speed = MoveSpeedMap.Contains(MoveComponent) ? MoveSpeedMap[MoveComponent] : 100.0f;

			MoveComponent->MoveDirection = Direction;
			MoveComponent->MoveSpeed = Speed;
			MoveComponent->StartMoving();
			UE_LOG(LogTemp, Warning, TEXT("Started movement for component: %s with direction: %s and speed: %f"),
				*MoveComponent->GetOwner()->GetName(),
				*Direction.ToString(),
				Speed);
		}
	}
}



// �� ��ư ������ ����
void UMovewidget::OnStopButtonClicked()
{
	if (SelectedActor)
	{
		UMyMoveActorComponent* MoveComponent = SelectedActor->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			MoveComponent->StopMoving();
		}
	}

	for (UMyMoveActorComponent* MoveComponent : ControlledMoveComponents)
	{
		if (MoveComponent)
		{
			MoveComponent->StopMoving();
		}
	}
}

// ���� �ڸ��� ���ư���
void UMovewidget::OnOriginButtonClicked()
{
	if (SelectedActor)
	{
		UMyMoveActorComponent* MoveComponent = SelectedActor->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			MoveComponent->OriginMove();
		}
	}

	for (UMyMoveActorComponent* MoveComponent : ControlledMoveComponents)
	{
		if (MoveComponent)
		{
			MoveComponent->OriginMove();
		}
	}
}



// �ӵ��� �Ÿ� �� set text
void UMovewidget::UpdateMovementValuesInUI(float SpeedValue, float DistanceValue)
{
	// �ӵ� ���� UI�� EditableText�� ����
	if (SpeedMoveText)
	{
		FText SpeedText = FText::AsNumber(SpeedValue); // ���ڸ� FText�� ��ȯ
		SpeedMoveText->SetText(SpeedText);
	}
	// �Ÿ� ���� UI�� EditableText�� ����
	if (DistMoveText)
	{
		FText DistText = FText::AsNumber(DistanceValue);
		DistMoveText->SetText(DistText);
	}
}

// �ӵ��� �Ÿ� �� �ִ� �Լ�
void UMovewidget::ApplyMovementValues()
{
	if (SelectedActor)
	{
		UMyMoveActorComponent* MoveComponent = SelectedActor->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			// �ӵ� �� �ִ� �κ�
			FString SpeedString = SpeedMoveText->GetText().ToString();
			float SpeedValue = FCString::Atof(*SpeedString);
			MoveSpeedMap.FindOrAdd(MoveComponent) = SpeedValue;

			// �Ÿ� �� �ִ� �κ�
			FString DistanceString = DistMoveText->GetText().ToString();
			float DistanceValue = FCString::Atof(*DistanceString);
			MoveComponent->MaxDistance = DistanceValue;

			UpdateMovementValuesInUI(SpeedValue, DistanceValue);
			UE_LOG(LogTemp, Warning, TEXT("Movement values applied for %s with speed: %f, distance: %f"),
				*MoveComponent->GetOwner()->GetName(), SpeedValue, DistanceValue);
		}
	}
}


// �ӵ� �Է� �ʵ忡�� ���͸� ���� ��
void UMovewidget::OnSpeedTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{	//  �ӵ� �Է� �� ó��
		FString SpeedString = Text.ToString();
		float SpeedValue = FCString::Atof(*SpeedString);

		if (AActor* Owner = GetOwnerFromComponent())
		{
			UMyMoveActorComponent* MoveComponent = Owner->FindComponentByClass<UMyMoveActorComponent>();
			if (MoveComponent)
			{
				MoveSpeedMap.FindOrAdd(MoveComponent) = SpeedValue;
				UE_LOG(LogTemp, Warning, TEXT("Speed set to: %f"), SpeedValue);
			}
		}
	}
}

void UMovewidget::OnDistanceTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		// �Էµ� �Ÿ� �� ó��
		FString DistanceString = Text.ToString();
		float DistanceValue = FCString::Atof(*DistanceString);
		UpdatePreviewLocation(StoredMoveDirection, DistanceValue);

		if (AActor* Owner = GetOwnerFromComponent())
		{
			UMyMoveActorComponent* MoveComponent = Owner->FindComponentByClass<UMyMoveActorComponent>();
			if (MoveComponent)
			{
				MoveComponent->MaxDistance = DistanceValue;
				UE_LOG(LogTemp, Warning, TEXT("Distance set to: %f"), DistanceValue);
			}
		}
	}
}

AActor* UMovewidget::GetOwnerFromComponent()
{
	// SelectedActor�� �����Ǿ� ������ �ٷ� ��ȯ
	if (SelectedActor)
	{
		return SelectedActor;
	}

	FStringClassReference BP_FunctionObjectClassRef(TEXT("/Game/YWK/BP/BP_MoveandFly.BP_MoveandFly_C"));
	UClass* BP_FunctionObjectClass = BP_FunctionObjectClassRef.TryLoadClass<AActor>();

	if (BP_FunctionObjectClass)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), BP_FunctionObjectClass, FoundActors);

		if (FoundActors.Num() > 0)
		{
			// ù ��° �߰ߵ� ���͸� �⺻���� ����
			SelectedActor = FoundActors[0];
			return SelectedActor;
		}
		// ������ ���� ����
		SelectedActor = GetWorld()->SpawnActor<AActor>(BP_FunctionObjectClass);
		return SelectedActor;
	}
	return nullptr;
}


// ��� ������Ʈ �ʱ�ȭ
void UMovewidget::InitializeFunctionObjects()
{
	FStringClassReference BP_FunctionObjectClassRef(TEXT("/Game/YWK/BP/BP_MoveandFly.BP_MoveandFly_C"));
	UClass* BP_FunctionObjectClass = BP_FunctionObjectClassRef.TryLoadClass<AActor>();

	if (BP_FunctionObjectClass && GetWorld())
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), BP_FunctionObjectClass, AllFunctionObject);

		// ComboBox�� �ʱ� ������Ʈ �߰�
		if (MoveBoxList)
		{
			for (AActor* FunctionObject : AllFunctionObject)
			{
				if (FunctionObject)
				{
					MoveBoxList->AddOption(FunctionObject->GetName());
				}
			}
			// �޺��ڽ� ���� ���� �� �̺�Ʈ ���ε�
			MoveBoxList->OnSelectionChanged.AddDynamic(this, &UMovewidget::OnFunctionObjectSelected);
		}
		// �迭�� ù ��° ������Ʈ�� �⺻ ����
		if (AllFunctionObject.Num() > 0)
		{
			SelectedActor = AllFunctionObject[0];
		}
	}
}

void UMovewidget::AddObjectToComboBox(AActor* NewObject)
{
	if (NewObject && MoveBoxList)
	{
		// ComboBox�� ������Ʈ �̸� �߰�
		FString DisplayName = NewObject->GetActorLabel(); // �����Ϳ��� ������ �̸�
		MoveBoxList->AddOption(DisplayName);

		// AllFunctionObject �迭�� ������Ʈ ���� �߰��Ͽ� ComboBox�� �ε����� ���߱�
		AllFunctionObject.Add(NewObject);
	}
}

void UMovewidget::OnFunctionObjectSelected(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	int32 SelectedIndex = MoveBoxList->FindOptionIndex(SelectedItem);
	if (SelectedIndex != INDEX_NONE && AllFunctionObject.IsValidIndex(SelectedIndex))
	{
		SelectedActor = AllFunctionObject[SelectedIndex];
		UE_LOG(LogTemp, Warning, TEXT("Selected function object: %s"), *SelectedActor->GetName());

		if (UMyMoveActorComponent* MoveComponent = SelectedActor->FindComponentByClass<UMyMoveActorComponent>())
		{
			// UI�� ���õ� MoveComponent�� ���� ������Ʈ
			UpdateMovementValuesInUI(MoveComponent->MoveSpeed, MoveComponent->MaxDistance);
			Chk_LoopMode->SetIsChecked(MoveComponent->bLoopMode);
			Chk_SingleDirectionMode->SetIsChecked(MoveComponent->bSingleDirection);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No matching function object found for: %s"), *SelectedItem);
	}
}


void UMovewidget::OnLoopModeCheckChanged(bool bIsChecked)
{
	if (SelectedActor)
	{
		UMyMoveActorComponent* MoveComponent = SelectedActor->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			MoveComponent->bLoopMode = bIsChecked;

			// Disable single direction if loop mode is enabled
			if (bIsChecked && Chk_SingleDirectionMode)
			{
				Chk_SingleDirectionMode->SetIsChecked(false);
				MoveComponent->bSingleDirection = false;
			}
			UE_LOG(LogTemp, Warning, TEXT("Loop mode set to: %s"), bIsChecked ? TEXT("Enabled") : TEXT("Disabled"));
		}
	}
}

void UMovewidget::OnSingleDirectionCheckChanged(bool bIsChecked)
{
	if (SelectedActor)
	{
		UMyMoveActorComponent* MoveComponent = SelectedActor->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			MoveComponent->bSingleDirection = bIsChecked;

			// Disable loop mode if single direction is enabled
			if (bIsChecked && Chk_LoopMode)
			{
				Chk_LoopMode->SetIsChecked(false);
				MoveComponent->bLoopMode = false;
			}
			UE_LOG(LogTemp, Warning, TEXT("Single Direction mode set to: %s"), bIsChecked ? TEXT("Enabled") : TEXT("Disabled"));
		}
	}
}

// ���� MoveComponent �߰� �Լ�
void UMovewidget::AddControlledMoveComponent(UMyMoveActorComponent* MoveComponent)
{
	if (MoveComponent && !ControlledMoveComponents.Contains(MoveComponent))
	{
		ControlledMoveComponents.Add(MoveComponent);
	}
}

// �������ϱ�
void UMovewidget::SetMoveDirection(FVector Direction)
{
	StoredMoveDirection = Direction;
	UpdatePreviewLocation(Direction, FCString::Atof(*DistMoveText->GetText().ToString()));
	if (SelectedActor)
	{
		UMyMoveActorComponent* MoveComponent = SelectedActor->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			MoveDirectionMap.FindOrAdd(MoveComponent) = Direction;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("StoredMoveDirection updated to: %s"), *StoredMoveDirection.ToString());
}

//void UMovewidget::SpawnPreviewActor()
//{
//	if (!PreviewActor)
//	{
//		FStringClassReference PreviewActorClassRef(TEXT("/Game/YWK/BP/BP_PreviewDistance.BP_PreviewDistance_C"));
//		UClass* PreviewActorClass = PreviewActorClassRef.TryLoadClass<AActor>();
//
//		if (PreviewActorClass)
//		{
//			PreviewActor = GetWorld()->SpawnActor<AActor>(PreviewActorClass, FVector::ZeroVector, FRotator::ZeroRotator);
//			if (PreviewActor)
//			{
//				PreviewActor->SetActorHiddenInGame(false);
//			}
//		}
//	}
//}

void UMovewidget::SpawnPreviewActor()
{
	if (!PreviewActor)
	{
		// BP_PreviewDistance ���� �ε� �� ����
		UClass* PreviewClass = LoadObject<UClass>(nullptr, TEXT("/Game/YWK/BP/BP_PreviewDistance.BP_PreviewDistance_C"));
		if (PreviewClass && SelectedActor)
		{
			// PreviewActor�� SelectedActor�� ��ġ�� ����
			PreviewActor = GetWorld()->SpawnActor<AActor>(PreviewClass);
			if (PreviewActor)
			{
				PreviewActor->SetActorLocation(SelectedActor->GetActorLocation()); // SelectedActor�� ��ġ�� ���� ��ġ
			}
		}
	}

	// ���� �Է°����� �̵� �ӵ��� �Ÿ� ��������
	if (SpeedMoveText)
	{
		FString SpeedString = SpeedMoveText->GetText().ToString();
		StoredMoveSpeed = FCString::Atof(*SpeedString);
	}

	if (DistMoveText)
	{
		FString DistanceString = DistMoveText->GetText().ToString();
		StoredMoveDistance = FCString::Atof(*DistanceString);
	}

	// �ʱ�ȭ: ���� ��ȯ ���� ����
	bPreviewDirectionReversed = false;

	// Ÿ�̸� ����
	if (PreviewActor)
	{
		GetWorld()->GetTimerManager().SetTimer(PreviewMoveTimer, this, &UMovewidget::UpdatePreviewMovement, 0.1f, true);
	}
}

void UMovewidget::UpdatePreviewMovement()
{
	if (!PreviewActor || !SelectedActor)
	{
		return;
	}

	FVector StartLocation = SelectedActor->GetActorLocation(); // ���� ��ġ�� �׻� SelectedActor�� ��ġ
	FVector TargetLocation = StartLocation + (StoredMoveDirection * StoredMoveDistance);

	FVector CurrentLocation = PreviewActor->GetActorLocation();
	FVector Direction = bPreviewDirectionReversed ? -StoredMoveDirection : StoredMoveDirection;

	// ���� ��ġ ���
	FVector NextLocation = CurrentLocation + Direction * StoredMoveSpeed * 0.1f;

	// ��ǥ�� �������� ��� ó��
	if (!bPreviewDirectionReversed && FVector::Dist(CurrentLocation, TargetLocation) <= 10.0f)
	{
		// ��ǥ������ �����ϸ� ���� ����
		bPreviewDirectionReversed = true;
	}
	else if (bPreviewDirectionReversed && FVector::Dist(CurrentLocation, StartLocation) <= 10.0f)
	{
		// ���������� �����ϸ� ���� �ʱ�ȭ
		bPreviewDirectionReversed = false;
	}

	// Preview Actor�� ��ġ ������Ʈ
	PreviewActor->SetActorLocation(NextLocation);
}

void UMovewidget::UpdatePreviewLocation(FVector Direction, float Distance)
{
	// ���� Preview Actor ����
	DestroyPreviewActor();

	// ���ο� ���� ����
	StoredMoveDirection = Direction;
	StoredMoveDistance = Distance;

	// Preview Actor ����
	SpawnPreviewActor();
}

void UMovewidget::DestroyPreviewActor()
{
	if (PreviewActor)
	{
		GetWorld()->GetTimerManager().ClearTimer(PreviewMoveTimer); // Ÿ�̸� ����
		PreviewActor->Destroy();
		PreviewActor = nullptr;
	}
}


