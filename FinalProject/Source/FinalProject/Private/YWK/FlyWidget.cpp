// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/FlyWidget.h"
#include "Components/Button.h"
#include "YWK/ActionChoice.h"
#include "Components/EditableText.h"
#include "Kismet/GameplayStatics.h"
#include "YWK/MyFlyActorComponent.h"

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
	
	// 속도 입력 필드에 OnTextCommitted 이벤트 바인딩
	if (FlySpeedText)
	{
		FlySpeedText->OnTextCommitted.AddDynamic(this, &UFlyWidget::OnFlySpeedTextCommitted);
	}

	// 거리 입력 필드에 OnTextCommitted 이벤트 바인딩
	if (FlyHightText)
	{
		FlyHightText->OnTextCommitted.AddDynamic(this, &UFlyWidget::OnFlyDistanceTextCommitted);
	}
}

// 위로 올라가기 버튼
void UFlyWidget::OnUpButtonClicked()
{
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyFlyActorComponent* FlyComponent = Owner->FindComponentByClass<UMyFlyActorComponent>();
		if (FlyComponent)
		{
			StoredFlyDirection = Owner->GetActorUpVector();
			UE_LOG(LogTemp, Warning, TEXT("Moving Up. New direction: %s"), *FlyComponent->FlyDirection.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FlyComponent not found!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Owner not found in OnUpButtonClicked!"));
	}
}

// 아래로 내려가기 버튼
void UFlyWidget::OnDownButtonClicked()
{
	// 컴포넌트 오너 가져오기
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyFlyActorComponent* FlyComponent = Owner->FindComponentByClass<UMyFlyActorComponent>();
		if (FlyComponent)
		{
			StoredFlyDirection = Owner->GetActorUpVector() * -1;

			// 로그 추가: 앞으로 이동 방향 설정 로그
			UE_LOG(LogTemp, Warning, TEXT("Moving Down. New direction: %s"), *FlyComponent->FlyDirection.ToString());
		}
	}
}

// 날기 시작
void UFlyWidget::OnStartButtonClicked()
{
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyFlyActorComponent* FlyComponent = Owner->FindComponentByClass<UMyFlyActorComponent>();
		if (FlyComponent)
		{
			// 저장된 방향으로 지정
			FlyComponent->FlyDirection = StoredFlyDirection;

			// 이동 시작 
			FlyComponent->StartFly();

			// 이동시작 로그 
			UE_LOG(LogTemp, Warning, TEXT("Starting Fly direction : %s"), *FlyComponent->FlyDirection.ToString());
		}
	}
}

// 날기 멈춤
void UFlyWidget::OnStopButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop button clicked"));

	// 컴포넌트 오너 가져오기
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyFlyActorComponent* FlyComponent = Owner->FindComponentByClass<UMyFlyActorComponent>();
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

// 원래 위치로 돌아가기
void UFlyWidget::OnOriginButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Origin button clicked"));

	// 컴포넌트 오너 가져오기
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

// 뒤로가기 버튼
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

// 속도와 높이 값 set
void UFlyWidget::UpdateMovementValuesInUI(float SpeedValue, float DistanceValue)
{
	// 속도 값을 UI의 EditableText에 설정
	if (FlySpeedText)
	{
		FText SpeedText = FText::AsNumber(SpeedValue); // 숫자를 FText로 변환
		FlySpeedText->SetText(SpeedText);
	}
	// 거리 값을 UI의 EditableText에 설정
	if (FlyHightText)
	{
		FText DistText = FText::AsNumber(DistanceValue);
		FlyHightText->SetText(DistText);
	}
}

// 속도나 거리 값 넣는 함수
void UFlyWidget::ApplyMovementValues()
{
	// 컴포넌트 가져오기
	if (AActor* Owner = GetOwnerFromComponent())
	{
		UMyFlyActorComponent* FlyComponent = Owner->FindComponentByClass<UMyFlyActorComponent>();
		if (FlyComponent)
		{
			// 속도값 설정
			FString SpeedString = FlySpeedText->GetText().ToString();
			float SpeedValue = FCString::Atof(*SpeedString); // 문자열 float로 바꾸기
			FlyComponent->FlySpeed = SpeedValue;

			// 거리값 설정
			FString DistnaceString = FlyHightText->GetText().ToString();
			float DistanceValue = FCString::Atof(*DistnaceString); // 이것도 float으로 바꾸기
			FlyComponent->FlyDistance = DistanceValue;

			// UI에 값 업데이트
			UpdateMovementValuesInUI(FlyComponent->FlySpeed, FlyComponent->FlyDistance);
		}
	}
}

// 속도 입력 필드에서 엔터를 쳤을 때
void UFlyWidget::OnFlySpeedTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		// 입력된 속도 값을 처리
		FString SpeedString = Text.ToString();
		float SpeedValue = FCString::Atof(*SpeedString);

		if (AActor* Owner = GetOwnerFromComponent())
		{
			UMyFlyActorComponent* FlyComponent = Owner->FindComponentByClass<UMyFlyActorComponent>();
			if (FlyComponent)
			{
				FlyComponent->FlySpeed = SpeedValue;
				// UI에 값 업데이트
				UE_LOG(LogTemp, Warning, TEXT("Speed set to: %f"), SpeedValue);
			}
		}
	}
}

// 높이 입력 필드에서 엔터를 쳤을 때
void UFlyWidget::OnFlyDistanceTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		// 입력된 거리 값을 처리
		FString DirectString = Text.ToString();
		float DistValues = FCString::Atof(*DirectString);

		if (AActor* Owner = GetOwnerFromComponent())
		{
			UMyFlyActorComponent* FlyComponent = Owner->FindComponentByClass<UMyFlyActorComponent>();
			if (FlyComponent)
			{
				FlyComponent->MaxFlyDistance = DistValues;
				// UI에 값 업데이트
				UE_LOG(LogTemp, Warning, TEXT("Distance set to: %f"), DistValues);
			}
		}
	}
}

AActor* UFlyWidget::GetOwnerFromComponent()
{
	// 블루프린트 액터 찾아오기..
	FStringClassReference BP_FunctionObjectClassRef(TEXT("/Game/YWK/BP/BP_FunctionObject.BP_FunctionObject_C"));

	// 블루프린트 클래스 로드
	UClass* BP_FunctionObjectClass = BP_FunctionObjectClassRef.TryLoadClass<AActor>();

	if (BP_FunctionObjectClass)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), BP_FunctionObjectClass, FoundActors);

		if (FoundActors.Num() > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found BP_FunctionObject: %s"), *FoundActors[0]->GetName());
			// 이미 존재하는 BP_FunctionObject 반환
			return FoundActors[0];
		}
		// 없으면 새로 스폰
		return GetWorld()->SpawnActor<AActor>(BP_FunctionObjectClass);
	}
	return nullptr;
	
}

