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
		UE_LOG(LogTemp, Error, TEXT("Owner is null in OnXClicked()"));
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

	// RotateComponent가 있으면 회전 시작
	if (RotateComponent)
	{
		RotateComponent->RotateDirection = FRotator(1.0f, 0.0f, 0.0f);  // X축 기준으로 회전
		RotateComponent->StartRolling();  // 회전 시작
	}
}


// Y축 회전
void URotationWidget::OnYClicked()
{
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyRotateActorComponent* RotateComponent = Owner->FindComponentByClass<UMyRotateActorComponent>();
		if (RotateComponent)
		{
			// Y축으로 회전 방향 설정
			RotateComponent->RotateDirection = FRotator(0.0f, 1.0f, 0.0f); // Y축 기준으로 회전
			RotateComponent->StartRolling(); // 회전 시작
		}
	}
}
// Z축 회전
void URotationWidget::OnZClicked()
{
	// GetOwnerFromComponent()로 가져온 Owner가 null인지 확인
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UE_LOG(LogTemp, Warning, TEXT("Owner found: %s"), *Owner->GetName());

		UMyRotateActorComponent* RotateComponent = Owner->FindComponentByClass<UMyRotateActorComponent>();
		if (RotateComponent)
		{
			// Z축으로 회전 방향 설정
			RotateComponent->RotateDirection = FRotator(0.0f, 0.0f, 1.0f); // Z축 기준으로 회전
			RotateComponent->StartRolling(); // 회전 시작
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
		return ControlledActor;
	}

	// 블루프린트 액터 찾아오기
	FStringClassReference BP_FunctionObjectClassRef(TEXT("/Game/YWK/BP/BP_FunctionObject1.BP_FunctionObject1_C"));

	// 블루프린트 클래스 로드
	UClass* BP_FunctionObjectClass = BP_FunctionObjectClassRef.TryLoadClass<AActor>();

	if (BP_FunctionObjectClass)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), BP_FunctionObjectClass, FoundActors);

		if (FoundActors.Num() > 0)
		{
			ControlledActor = FoundActors[0];
			UE_LOG(LogTemp, Warning, TEXT("Found BP_FunctionObject: %s"), *FoundActors[0]->GetName());
			// 이미 존재하는 BP_FunctionObject 반환
			return ControlledActor;
		}
		// 없으면 새로 스폰
		ControlledActor = GetWorld()->SpawnActor<AActor>(BP_FunctionObjectClass);
		return ControlledActor;
	}
	return nullptr;
}