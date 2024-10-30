// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/Movewidget.h"
#include "Components/Button.h"
#include "YWK/ActionChoice.h"
#include "YWK/MyMoveActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/EditableText.h"

void UMovewidget::NativeConstruct()
{
	Super::NativeConstruct();

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

}

// ������ ����
void UMovewidget::OnFrontClicked()
{
	// ������Ʈ ���� ��������
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyMoveActorComponent* MoveComponent = Owner->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			StoredMoveDirection = Owner->GetActorForwardVector();

			// �α� �߰�: ������ �̵� ���� ���� �α�
			UE_LOG(LogTemp, Warning, TEXT("Moving forward. New direction: %s"), *MoveComponent->MoveDirection.ToString());
		}
	}
}

// �ڷ� ����
void UMovewidget::OnBackClicked()
{
	// ������Ʈ ���� ��������
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyMoveActorComponent* MoveComponent = Owner->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			StoredMoveDirection = Owner->GetActorForwardVector() * -1;

			// �α� �߰�: �ڷ� �̵� ���� ���� �α�
			UE_LOG(LogTemp, Warning, TEXT("Moving Backward. New direction: %s"), *MoveComponent->MoveDirection.ToString());
		}
	}
}

// ���������� ����
void UMovewidget::OnRightClicked()
{
	// ������Ʈ ���� ��������
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyMoveActorComponent* MoveComponent = Owner->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			StoredMoveDirection = Owner->GetActorRightVector();

			// �α� �߰�: ���������� �̵� ���� ���� �α�
			UE_LOG(LogTemp, Warning, TEXT("Moving Right. New direction: %s"), *MoveComponent->MoveDirection.ToString());
		}
	}
}

// �������� ����
void UMovewidget::OnLeftClicked()
{
	// ������Ʈ ���� ��������
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyMoveActorComponent* MoveComponent = Owner->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			StoredMoveDirection = Owner->GetActorRightVector() * -1;

			// �α� �߰�: �������� �̵� ���� ���� �α�
			UE_LOG(LogTemp, Warning, TEXT("Moving Left. New direction: %s"), *MoveComponent->MoveDirection.ToString());
		}
	}
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

// �� ��ư ������ �����̴°� ���۵�
void UMovewidget::OnStartButtonClicked()
{
	//RemoveFromParent();

	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyMoveActorComponent* MoveComponent = Owner->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			// ����� �������� ����
			MoveComponent->MoveDirection = StoredMoveDirection;

			// �̵� ���� 
			MoveComponent->StartMoving();

			// �̵����� �α� 
			UE_LOG(LogTemp, Warning, TEXT("Starting move direction : %s"), *MoveComponent->MoveDirection.ToString());
		}
	}
}

// �� ��ư ������ ����
void UMovewidget::OnStopButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop button clicked"));

	// ������Ʈ ���� ��������
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyMoveActorComponent* MoveComponent = Owner->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			MoveComponent->StopMoving();
			UE_LOG(LogTemp, Warning, TEXT("Stop button clicked: calling StopMoving()"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MoveComponent not found"));
		}
	}
}

// ���� �ڸ��� ���ư���
void UMovewidget::OnOriginButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Origin button clicked"));

	// ������Ʈ ���� ��������
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyMoveActorComponent* MoveComponent = Owner->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			MoveComponent->OriginMove();
			UE_LOG(LogTemp, Warning, TEXT("Origin button clicked: calling OriginMove()"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MoveComponent not found"));
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
	// ������Ʈ ��������
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyMoveActorComponent* MoveComponent = Owner->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			// �ӵ��� ����
			FString SpeedString = SpeedMoveText->GetText().ToString();
			float SpeedValue = FCString::Atof(*SpeedString); // ���ڿ� float�� �ٲٱ�
			MoveComponent->MoveSpeed = SpeedValue;

			// �Ÿ��� ����
			FString DistnaceString = DistMoveText->GetText().ToString();
			float DistanceValue = FCString::Atof(*DistnaceString); // �̰͵� float���� �ٲٱ�
			MoveComponent->MaxDistance = DistanceValue;

			// UI�� �� ������Ʈ
			UpdateMovementValuesInUI(MoveComponent->MoveSpeed, MoveComponent->MoveDistance);
		}
	}
}


// �ӵ� �Է� �ʵ忡�� ���͸� ���� ��
void UMovewidget::OnSpeedTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		// �Էµ� �ӵ� ���� ó��
		FString SpeedString = Text.ToString();
		float SpeedValue = FCString::Atof(*SpeedString);

		if (AActor* Owner = GetOwnerFromComponent())
		{
			UMyMoveActorComponent* MoveComponent = Owner->FindComponentByClass<UMyMoveActorComponent>();
			if (MoveComponent)
			{
				MoveComponent->MoveSpeed = SpeedValue;
				// UI�� �� ������Ʈ
				UE_LOG(LogTemp, Warning, TEXT("Speed set to: %f"), SpeedValue);
			}
		}
	}
}

void UMovewidget::OnDistanceTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		// �Էµ� �Ÿ� ���� ó��
		FString DirectString = Text.ToString();
		float DistValues = FCString::Atof(*DirectString);

		if (AActor* Owner = GetOwnerFromComponent())
		{
			UMyMoveActorComponent* MoveComponent = Owner->FindComponentByClass<UMyMoveActorComponent>();
			if (MoveComponent)
			{
				MoveComponent->MaxDistance = DistValues;
				// UI�� �� ������Ʈ
				UE_LOG(LogTemp, Warning, TEXT("Speed set to: %f"), DistValues);
			}
		}
	}
}

AActor* UMovewidget::GetOwnerFromComponent()
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
	return nullptr;
}
