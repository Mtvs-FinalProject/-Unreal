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

	// ui뒤로가기 버튼
	if (Btn_RotateBack)
	{
		Btn_RotateBack->OnClicked.AddDynamic(this, &URotationWidget::OnRotateBackClicked);
	}
	
	// X축으로 돌기
	if (Btn_X)
	{
		Btn_X->OnClicked.AddDynamic(this, &URotationWidget::OnXClicked);
	}
	
	// y축으로 돌기
	if (Btn_Y)
	{
		Btn_Y->OnClicked.AddDynamic(this, &URotationWidget::OnYClicked);
	}

	// z축으로 돌기
	if (Btn_Z)
	{
		Btn_Z->OnClicked.AddDynamic(this, &URotationWidget::OnZClicked);
	}

	// 회전 시작
	if (Btn_RotateStart)
	{
		Btn_RotateStart->OnClicked.AddDynamic(this, &URotationWidget::OnRotateStartClicked);
	}

	// 회전 정지
	if (Btn_RotateStop)
	{
		Btn_RotateStop->OnClicked.AddDynamic(this, &URotationWidget::OnRotateStopClicked);
	}

	// 원위치로 돌기
	if (Btn_RotateOrigin)
	{
		Btn_RotateOrigin->OnClicked.AddDynamic(this, &URotationWidget::OnRotateOriginClicked);
	}
}

// X축 회전
void URotationWidget::OnXClicked()
{
	AActor* Owner = GetOwnerFromComponent();  // Owner 액터를 가져옴
	if (!Owner)
	{
		UE_LOG(LogTemp, Log, TEXT("Owner is null in OnXClicked()"));
		return;  // Owner가 null이면 더 이상 진행하지 않음
	}

	// UMyRotateActorComponent 찾기
	UMyRotateActorComponent* RotateComponent = Owner->FindComponentByClass<UMyRotateActorComponent>();

	// RotateComponent가 없으면 동적으로 추가
	if (!RotateComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("RotateComponent is null. Adding a new MyRotateActorComponent."));
		RotateComponent = NewObject<UMyRotateActorComponent>(Owner);
		if (RotateComponent)
		{
			Owner->AddInstanceComponent(RotateComponent);  // 컴포넌트를 액터에 추가
			RotateComponent->RegisterComponent();  // 컴포넌트를 등록
		}
	}

	// RotatorComponent가 있다면 회전 방향만 바꿈
	if (RotateComponent)
	{
		RotateComponent->RotateDirection = FRotator(1.0f, 0.0f, 0.0f);
		UE_LOG(LogTemp, Log, TEXT("X-axis rotation direction set."));
	}
}


// Y축 회전
void URotationWidget::OnYClicked()
{
	AActor* Owner = GetOwnerFromComponent();
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner is null in OnYClicked"));
		return;  // Owner가 null이면 더 이상 진행하지 않음
	}

	UMyRotateActorComponent* RotateComponent = Owner->FindComponentByClass<UMyRotateActorComponent>();
	if (!RotateComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("RotateComponent not found in OnYClicked, creating new one"));
		RotateComponent = NewObject<UMyRotateActorComponent>(Owner);
		if (RotateComponent)
		{
			Owner->AddInstanceComponent(RotateComponent); // 컴포넌트를 액터에 추가
			RotateComponent->RegisterComponent(); // 컴포넌트 등록
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create RotateComponent in OnYClicked"));
			return;  // 생성 실패 시 반환
		}
	}

	if (RotateComponent)
	{
		RotateComponent->RotateDirection = FRotator(0.0f, 1.0f, 0.0f);
		UE_LOG(LogTemp, Log, TEXT("Y-axis rotation direction set."));
	}
}



// Z축 회전
void URotationWidget::OnZClicked()
{
	AActor* Owner = GetOwnerFromComponent();
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner is null in OnZClicked"));
		return;
	}

	UMyRotateActorComponent* RotateComponent = Owner->FindComponentByClass<UMyRotateActorComponent>();
	if (!RotateComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("RotateComponent not found in OnZClicked"));
		RotateComponent = NewObject<UMyRotateActorComponent>(Owner);
		if (RotateComponent)
		{
			Owner->AddInstanceComponent(RotateComponent); // 컴포넌트를 액터에 추가
			RotateComponent->RegisterComponent(); // 컴포넌트 등록
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create RotateComponent in OnZClicked"));
			return; // 컴포넌트 생성에 실패한 경우, 더 진행하지 않음
		}
	}

	// RotatorComponent가 있다면 회전 방향만 바꿈
	if (RotateComponent)
	{
		RotateComponent->RotateDirection = FRotator(0.0f, 0.0f, 1.0f);
		UE_LOG(LogTemp, Log, TEXT("Z-axis rotation direction set."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("RotateComponent is still null after creation attempt in OnZClicked"));
	}
}


// ui 뒤로가기
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
// 회전 시작
void URotationWidget::OnRotateStartClicked()
{
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyRotateActorComponent* RotateComponent = Owner->FindComponentByClass<UMyRotateActorComponent>();
		if (RotateComponent)
		{
			RotateComponent->StartRolling();
			UE_LOG(LogTemp, Warning, TEXT("Rotation started."));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("RotateComponent not found."));
		}
	}
}
// 회전 초기화
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
		// 입력된 속도 값을 처리
		FString SpeedString = Text.ToString();
		float SpeedValue = FCString::Atof(*SpeedString);

		if (AActor* Owner = GetOwnerFromComponent())
		{
			UMyRotateActorComponent* RotateComponent = Owner->FindComponentByClass<UMyRotateActorComponent>();
			if (RotateComponent)
			{
				RotateComponent->RotateSpeed = SpeedValue;
				// UI에 값 업데이트
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
		UE_LOG(LogTemp, Log, TEXT("ControlledActor already set: %s"), *ControlledActor->GetName());
		return ControlledActor;
	}

	UE_LOG(LogTemp, Warning, TEXT("No ControlledActor, searching for BP_FunctionObject"));

	// 블루프린트 액터 찾아오기
	FStringClassReference BP_FunctionObjectClassRef(TEXT("/Game/YWK/BP/BP_FunctionObject1.BP_FunctionObject1_C"));
	UClass* BP_FunctionObjectClass = BP_FunctionObjectClassRef.TryLoadClass<AActor>();

	if (!BP_FunctionObjectClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load BP_FunctionObject class"));
		return nullptr;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), BP_FunctionObjectClass, FoundActors);

	if (FoundActors.Num() > 0)
	{
		ControlledActor = FoundActors[0];
		UE_LOG(LogTemp, Warning, TEXT("Found BP_FunctionObject: %s"), *FoundActors[0]->GetName());
		return ControlledActor;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BP_FunctionObject not found, attempting to spawn"));

		// 없으면 새로 스폰
		ControlledActor = GetWorld()->SpawnActor<AActor>(BP_FunctionObjectClass);
		if (ControlledActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawned new BP_FunctionObject: %s"), *ControlledActor->GetName());
			return ControlledActor;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn BP_FunctionObject"));
			return nullptr;
		}
	}
}
