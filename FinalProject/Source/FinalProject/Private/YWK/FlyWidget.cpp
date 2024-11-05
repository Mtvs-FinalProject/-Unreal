// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/FlyWidget.h"
#include "Components/Button.h"
#include "YWK/ActionChoice.h"
#include "Components/EditableText.h"
#include "Kismet/GameplayStatics.h"
#include "YWK/MyFlyActorComponent.h"
#include "Components/ComboBoxString.h"

void UFlyWidget::NativeConstruct()
{
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
}

// ���� �ö󰡱� ��ư
void UFlyWidget::OnUpButtonClicked()
{
	if (SelectedActor)
	{
		UMyFlyActorComponent* FlyComponent = SelectedActor->FindComponentByClass<UMyFlyActorComponent>();
		if (FlyComponent)
		{
			StoredFlyDirection = SelectedActor->GetActorUpVector();
			UE_LOG(LogTemp, Warning, TEXT("Moving Up. New direction: %s"), *FlyComponent->FlyDirection.ToString());
		}
	}
}

// �Ʒ��� �������� ��ư
void UFlyWidget::OnDownButtonClicked()
{
	// ������Ʈ ���� ��������
	if (SelectedActor)
	{
		UMyFlyActorComponent* FlyComponent = SelectedActor->FindComponentByClass<UMyFlyActorComponent>();
		if (FlyComponent)
		{
			StoredFlyDirection = SelectedActor->GetActorUpVector() * -1;

			// �α� �߰�: ������ �̵� ���� ���� �α�
			UE_LOG(LogTemp, Warning, TEXT("Moving Down. New direction: %s"), *FlyComponent->FlyDirection.ToString());
		}
	}
}

// ���� ����
void UFlyWidget::OnStartButtonClicked()
{
	if (SelectedActor)
	{
		UMyFlyActorComponent* FlyComponent = SelectedActor->FindComponentByClass<UMyFlyActorComponent>();
		if (FlyComponent)
		{
			// ����� �������� ����
			FlyComponent->FlyDirection = StoredFlyDirection;

			// �̵� ���� 
			FlyComponent->StartFly();

			// �̵����� �α� 
			UE_LOG(LogTemp, Warning, TEXT("Starting Fly direction : %s"), *FlyComponent->FlyDirection.ToString());
		}
	}
}

// ���� ����
void UFlyWidget::OnStopButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop button clicked"));

	// ������Ʈ ���� ��������
	if (SelectedActor)
	{
		UMyFlyActorComponent* FlyComponent = SelectedActor->FindComponentByClass<UMyFlyActorComponent>();
		if (FlyComponent)
		{
			FlyComponent->StopFly();
			UE_LOG(LogTemp, Warning, TEXT("Stop button clicked: calling StopFly()"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("FlyComponent not found"));
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
void UFlyWidget::UpdateMovementValuesInUI(float SpeedValue, float DistanceValue)
{
	// �ӵ� ���� UI�� EditableText�� ����
	if (FlySpeedText)
	{
		FText SpeedText = FText::AsNumber(SpeedValue); // ���ڸ� FText�� ��ȯ
		FlySpeedText->SetText(SpeedText);
	}
	// �Ÿ� ���� UI�� EditableText�� ����
	if (FlyHightText)
	{
		FText DistText = FText::AsNumber(DistanceValue);
		FlyHightText->SetText(DistText);
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
		FString DirectString = Text.ToString();
		float DistValues = FCString::Atof(*DirectString);

		if (AActor* Owner = GetOwnerFromComponent())
		{
			UMyFlyActorComponent* FlyComponent = Owner->FindComponentByClass<UMyFlyActorComponent>();
			if (FlyComponent)
			{
				FlyComponent->MaxFlyDistance = DistValues;
				// UI�� �� ������Ʈ
				UE_LOG(LogTemp, Warning, TEXT("Distance set to: %f"), DistValues);
			}
		}
	}
}

AActor* UFlyWidget::GetOwnerFromComponent()
{
	// �������Ʈ ���� ã�ƿ���..
	FStringClassReference BP_FunctionObjectClassRef(TEXT("/Game/YWK/BP/BP_MoveandFly.BP_MoveandFly_C"));

	// �������Ʈ Ŭ���� �ε�
	UClass* BP_FunctionObjectClass = BP_FunctionObjectClassRef.TryLoadClass<AActor>();

	if (BP_FunctionObjectClass) 

	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), BP_FunctionObjectClass, FoundActors);

		if (FoundActors.Num() > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found BP_FunctionObject: %s"), *FoundActors[0]->GetName());
			// �̹� �����ϴ� BP_FunctionObject ��ȯ
			return FoundActors[0];
		}
		// ������ ���� ����
		return GetWorld()->SpawnActor<AActor>(BP_FunctionObjectClass);
	}
	return SelectedActor;
	
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

void UFlyWidget::OnFunctionObjectSelected(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	int32 SelectedIndex = FlyBoxList->FindOptionIndex(SelectedItem);
	if (SelectedIndex != INDEX_NONE && AllFunctionObject.IsValidIndex(SelectedIndex))
	{
		SelectedActor = AllFunctionObject[SelectedIndex];
		UE_LOG(LogTemp, Warning, TEXT("Selected fly function object: %s"), *SelectedActor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No matching function object found for: %s"), *SelectedItem);
	}
}

