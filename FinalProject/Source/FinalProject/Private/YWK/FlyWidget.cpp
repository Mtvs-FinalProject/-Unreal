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
	// Initialize function objects in ComboBox
	InitializeFunctionObjects();

	// 왕복모드 체크 박스
	if (Chk_LoopMode)
	{
		Chk_LoopMode->OnCheckStateChanged.AddDynamic(this, &UFlyWidget::OnLoopModeCheckChanged);
	}

	// 일방모드 체크 박스
	if (Chk_SingleDirectionMode)
	{
		Chk_SingleDirectionMode->OnCheckStateChanged.AddDynamic(this, &UFlyWidget::OnSingleDirectionCheckChanged);
	}

	// 왕복 텍스트 박스
	if (Txt_LoopCount)
	{
		Txt_LoopCount->OnTextCommitted.AddDynamic(this, &UFlyWidget::OnLoopCountCommitted);
	}
}

// 위로 올라가기 버튼
void UFlyWidget::OnUpButtonClicked()
{
	StoredFlyDirection = FVector(0.0f, 0.0f, 1.0f);
	UpdatePreviewLocation(StoredFlyDirection, FCString::Atof(*FlyHightText->GetText().ToString()));
	UE_LOG(LogTemp, Warning, TEXT("Fly direction set to Up."));
}

// 아래로 내려가기 버튼
void UFlyWidget::OnDownButtonClicked()
{
	StoredFlyDirection = FVector(0.0f, 0.0f, -1.0f);
	UpdatePreviewLocation(StoredFlyDirection, FCString::Atof(*FlyHightText->GetText().ToString()));
	UE_LOG(LogTemp, Warning, TEXT("Fly direction set to Down."));
}

// 날기 시작
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

// 날기 멈춤
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
void UFlyWidget::UpdateMovementValuesInUI(float SpeedValue = -1.0f, float DistanceValue = -1.0f)
{
	// 속도 값을 UI의 EditableText에 설정
	if (FlySpeedText)
	{
		// SpeedValue가 음수(-1.0f)일 경우 비워두고, 그렇지 않으면 값 설정
		FlySpeedText->SetText(SpeedValue < 0.0f ? FText::GetEmpty() : FText::AsNumber(SpeedValue));
	}
	// 거리 값을 UI의 EditableText에 설정
	if (FlyHightText)
	{
		// DistanceValue가 음수(-1.0f)일 경우 비워두고, 그렇지 않으면 값 설정
		FlyHightText->SetText(DistanceValue < 0.0f ? FText::GetEmpty() : FText::AsNumber(DistanceValue));
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
		float DistValues = FCString::Atof(*Text.ToString());

		if (AActor* Owner = GetOwnerFromComponent())
		{
			UMyFlyActorComponent* FlyComponent = Owner->FindComponentByClass<UMyFlyActorComponent>();
			if (FlyComponent)
			{
				FlyComponent->MaxFlyDistance = DistValues;
				UE_LOG(LogTemp, Warning, TEXT("Distance set to: %f"), DistValues);
				UpdatePreviewLocation(StoredFlyDirection, DistValues); // 미리보기 위치 업데이트 추가
			}
		}
	}
}

AActor* UFlyWidget::GetOwnerFromComponent()
{
	if (SelectedActor)
	{
		return SelectedActor; // 선택한 액터가 있으면 바로 반환
	}

	// 선택한 액터가 없을 경우, BP_FunctionObject 클래스에서 찾음
	FStringClassReference BP_FunctionObjectClassRef(TEXT("/Game/YWK/BP/BP_MoveandFly.BP_MoveandFly_C"));
	UClass* BP_FunctionObjectClass = BP_FunctionObjectClassRef.TryLoadClass<AActor>();

	if (BP_FunctionObjectClass)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), BP_FunctionObjectClass, FoundActors);

		if (FoundActors.Num() > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found BP_FunctionObject: %s"), *FoundActors[0]->GetName());
			SelectedActor = FoundActors[0]; // 첫 번째 액터를 선택된 액터로 설정
			return SelectedActor;
		}

		// 없으면 새로 스폰
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
		//Display name을 추가할 때 Object의 Label을 사용
		FString DisplayName = NewObject->GetActorLabel();
		FlyBoxList->AddOption(DisplayName);

		//AllFunctionObject 배열에 추가하여 나중에 참조 가능하게 
		AllFunctionObject.Add(NewObject);
		UE_LOG(LogTemp, Warning, TEXT("Added new object to ComboBox: %s"), *DisplayName);
	}
}

// 선택한 오브젝트에 따라 개별 FlyComponent를 설정
void UFlyWidget::OnFunctionObjectSelected(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	int32 SelectedIndex = FlyBoxList->FindOptionIndex(SelectedItem);
	if (SelectedIndex != INDEX_NONE && AllFunctionObject.IsValidIndex(SelectedIndex))
	{
		SelectedActor = AllFunctionObject[SelectedIndex];
		UE_LOG(LogTemp, Warning, TEXT("Selected fly function object: %s"), *SelectedActor->GetName());

		// 선택된 오브젝트의 FlyComponent 설정
		if (UMyFlyActorComponent* FlyComponent = SelectedActor->FindComponentByClass<UMyFlyActorComponent>())
		{
			// UI 요소와 FlyComponent 값 동기화 대신 초기 상태로 비움
			UpdateMovementValuesInUI(); // 파라미터 전달하지 않음으로써 기본값 설정
			Chk_LoopMode->SetIsChecked(FlyComponent->bLoopMode);
			Chk_SingleDirectionMode->SetIsChecked(FlyComponent->bSingleDirection);

			// 왕복 횟수 업데이트
			if (Txt_LoopCount)
			{
				Txt_LoopCount->SetText(FText::AsNumber(FlyComponent->LoopCount));
			}
		}
	}
}

// 왕복 모드 체크박스 상태 변경
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

// 단순 이동 모드 체크박스 상태 변경
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

// 왕복 횟수 입력 필드에서 엔터를 쳤을 때
void UFlyWidget::OnLoopCountCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter && SelectedActor)
	{
		if (UMyFlyActorComponent* FlyComponent = SelectedActor->FindComponentByClass<UMyFlyActorComponent>())
		{
			int32 LoopValue = FCString::Atoi(*Text.ToString());
			FlyComponent->LoopCount = LoopValue;
			FlyComponent->CurrentLoop = 0;  // 새로운 루프 설정 시 루프 초기화
			UE_LOG(LogTemp, Warning, TEXT("Loop count set to: %d for %s"), LoopValue, *SelectedActor->GetName());
		}
	}
}

// 다중 FlyComponent 추가 함수
void UFlyWidget::AddControlledFlyComponent(UMyFlyActorComponent* NewFlyComponent)
{
	if (NewFlyComponent && !ControlledFlyComponents.Contains(NewFlyComponent))
	{
		ControlledFlyComponents.Add(NewFlyComponent);
	}
}

// 프리뷰 액터 스폰 함수
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

// 프리뷰 위치 업데이트 함수
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

