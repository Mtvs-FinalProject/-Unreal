// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/RotationWidget.h"
#include "YWK/ActionChoice.h"
#include "YWK/MyRotateActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"

void URotationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// ui�ڷΰ��� ��ư
	if (Btn_RotateBack)
	{
		Btn_RotateBack->OnClicked.AddDynamic(this, &URotationWidget::OnRotateBackClicked);
	}
	
	// X������ ����
	if (Btn_X)
	{
		Btn_X->OnClicked.AddDynamic(this, &URotationWidget::OnXClicked);
	}
	
	// y������ ����
	if (Btn_Y)
	{
		Btn_Y->OnClicked.AddDynamic(this, &URotationWidget::OnYClicked);
	}

	// z������ ����
	if (Btn_Z)
	{
		Btn_Z->OnClicked.AddDynamic(this, &URotationWidget::OnZClicked);
	}

	// ȸ�� ����
	if (Btn_RotateStart)
	{
		Btn_RotateStart->OnClicked.AddDynamic(this, &URotationWidget::OnRotateStartClicked);
	}

	// ȸ�� ����
	if (Btn_RotateStop)
	{
		Btn_RotateStop->OnClicked.AddDynamic(this, &URotationWidget::OnRotateStopClicked);
	}

	// ����ġ�� ����
	if (Btn_RotateOrigin)
	{
		Btn_RotateOrigin->OnClicked.AddDynamic(this, &URotationWidget::OnRotateOriginClicked);
	}
}

// X�� ȸ��
void URotationWidget::OnXClicked()
{
	AActor* Owner = GetOwnerFromComponent();  // Owner ���͸� ������
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner is null in OnXClicked()"));
		return;  // Owner�� null�̸� �� �̻� �������� ����
	}

	// UMyRotateActorComponent ã��
	UMyRotateActorComponent* RotateComponent = Owner->FindComponentByClass<UMyRotateActorComponent>();

	// RotateComponent�� ������ �������� �߰�
	if (!RotateComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("RotateComponent is null. Adding a new MyRotateActorComponent."));
		RotateComponent = NewObject<UMyRotateActorComponent>(Owner);
		if (RotateComponent)
		{
			Owner->AddInstanceComponent(RotateComponent);  // ������Ʈ�� ���Ϳ� �߰�
			RotateComponent->RegisterComponent();  // ������Ʈ�� ���
		}
	}

	// RotateComponent�� ������ ȸ�� ����
	if (RotateComponent)
	{
		RotateComponent->RotateDirection = FRotator(1.0f, 0.0f, 0.0f);  // X�� �������� ȸ��
		RotateComponent->StartRolling();  // ȸ�� ����
	}
}


// Y�� ȸ��
void URotationWidget::OnYClicked()
{
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyRotateActorComponent* RotateComponent = Owner->FindComponentByClass<UMyRotateActorComponent>();
		if (RotateComponent)
		{
			// Y������ ȸ�� ���� ����
			RotateComponent->RotateDirection = FRotator(0.0f, 1.0f, 0.0f); // Y�� �������� ȸ��
			RotateComponent->StartRolling(); // ȸ�� ����
		}
	}
}
// Z�� ȸ��
void URotationWidget::OnZClicked()
{
	// GetOwnerFromComponent()�� ������ Owner�� null���� Ȯ��
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UE_LOG(LogTemp, Warning, TEXT("Owner found: %s"), *Owner->GetName());

		UMyRotateActorComponent* RotateComponent = Owner->FindComponentByClass<UMyRotateActorComponent>();
		if (RotateComponent)
		{
			// Z������ ȸ�� ���� ����
			RotateComponent->RotateDirection = FRotator(0.0f, 0.0f, 1.0f); // Z�� �������� ȸ��
			RotateComponent->StartRolling(); // ȸ�� ����
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("RotateComponent not found!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Owner not found in OnZClicked!"));
	}
}

// ui �ڷΰ���
void URotationWidget::OnRotateBackClicked()
{
	RemoveFromParent();

	if (ActionChoice)
	{
		UUserWidget* BackRotateWidget = CreateWidget<UUserWidget>(GetWorld(), ActionChoice);
		if (BackRotateWidget)
		{
			BackRotateWidget->AddToViewport();
		}
	}
}
// ȸ�� ����
void URotationWidget::OnRotateStartClicked()
{
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyRotateActorComponent* RotateComponent = Owner->FindComponentByClass<UMyRotateActorComponent>();
		if (RotateComponent)
		{
			RotateComponent->StartRolling();
		}
	}
}
// ȸ�� �ʱ�ȭ
void URotationWidget::OnRotateOriginClicked()
{
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyRotateActorComponent* RotateComponent = Owner->FindComponentByClass<UMyRotateActorComponent>();
		if (RotateComponent)
		{
			RotateComponent->OriginRolling();
		}
	}
}

void URotationWidget::OnRotateStopClicked()
{
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyRotateActorComponent* RotateComponent = Owner->FindComponentByClass<UMyRotateActorComponent>();
		if (RotateComponent)
		{
			RotateComponent->StopRolling();
		}
	}
}

void URotationWidget::UpdateSpeedTextUI(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		// �Էµ� �ӵ� ���� ó��
		FString SpeedString = Text.ToString();
		float SpeedValue = FCString::Atof(*SpeedString);

		if (AActor* Owner = GetOwnerFromComponent())
		{
			UMyRotateActorComponent* RotateComponent = Owner->FindComponentByClass<UMyRotateActorComponent>();
			if (RotateComponent)
			{
				RotateComponent->RotateSpeed = SpeedValue;
				// UI�� �� ������Ʈ
				UE_LOG(LogTemp, Warning, TEXT("Speed set to: %f"), SpeedValue);
			}
		}
	}
}

AActor* ControlledActor = nullptr;

AActor* URotationWidget::GetOwnerFromComponent()
{
	if (ControlledActor)
	{
		return ControlledActor;
	}

	// �������Ʈ ���� ã�ƿ���
	FStringClassReference BP_FunctionObjectClassRef(TEXT("/Game/YWK/BP/BP_FunctionObject1.BP_FunctionObject1_C"));

	// �������Ʈ Ŭ���� �ε�
	UClass* BP_FunctionObjectClass = BP_FunctionObjectClassRef.TryLoadClass<AActor>();

	if (BP_FunctionObjectClass)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), BP_FunctionObjectClass, FoundActors);

		if (FoundActors.Num() > 0)
		{
			ControlledActor = FoundActors[0];
			UE_LOG(LogTemp, Warning, TEXT("Found BP_FunctionObject: %s"), *FoundActors[0]->GetName());
			// �̹� �����ϴ� BP_FunctionObject ��ȯ
			return ControlledActor;
		}
		// ������ ���� ����
		ControlledActor = GetWorld()->SpawnActor<AActor>(BP_FunctionObjectClass);
		return ControlledActor;
	}
	return nullptr;
}