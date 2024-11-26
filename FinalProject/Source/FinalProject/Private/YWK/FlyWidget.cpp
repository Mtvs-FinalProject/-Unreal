// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/FlyWidget.h"
#include "Components/Button.h"
#include "YWK/ActionChoice.h"
#include "Components/EditableText.h"
#include "Kismet/GameplayStatics.h"
#include "YWK/MyFlyActorComponent.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "../FinalProject.h"


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

	// 콤보박스 글자 색 바꾸기
	if (FlyBoxList)
	{
		FlyBoxList->OnGenerateWidgetEvent.BindDynamic(this, &UFlyWidget::GenerateComboBoxItem);
	}
}

// 위로 올라가기 버튼
void UFlyWidget::OnUpButtonClicked()
{
	StoredFlyDirection = FVector(0.0f, 0.0f, 1.0f);
	UpdatePreviewLocation(StoredFlyDirection, FCString::Atof(*FlyHightText->GetText().ToString()));

	if (SelectedActor)
	{
		UMyFlyActorComponent* FlyComponent = SelectedActor->FindComponentByClass<UMyFlyActorComponent>();
		if (FlyComponent)
		{
			FlyComponent->FlyDirection =  FVector::UpVector;

		}
	}
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
			/*FlyComponent->FlyDirection = StoredFlyDirection.IsNearlyZero() ? FVector(0.0f, 0.0f, 1.0f) : StoredFlyDirection;*/
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
void UFlyWidget::OnFlyDistanceTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		// 거리 값을 설정
		float DistValues = FCString::Atof(*Text.ToString());
		StoredMoveDistance = (DistValues > 0.0f) ? DistValues : 1000.0f;

		UE_LOG(LogTemp, Warning, TEXT("Distance set to: %f"), StoredMoveDistance);

		// 선택된 오브젝트에 거리 값 적용
		if (SelectedActor)
		{
			if (UMyFlyActorComponent* FlyComponent = SelectedActor->FindComponentByClass<UMyFlyActorComponent>())
			{
				FlyComponent->FlyDistance = StoredMoveDistance;
				UE_LOG(LogTemp, Warning, TEXT("Distance applied to actor: %f"), FlyComponent->FlyDistance);
			}
		}

		// 프리뷰 위치 업데이트
		UpdatePreviewLocation(StoredFlyDirection, StoredMoveDistance);
	}
}



// 높이 입력 필드에서 엔터를 쳤을 때
void UFlyWidget::OnFlySpeedTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		// 속도 값을 설정
		float SpeedValue = FCString::Atof(*Text.ToString());
		StoredMoveSpeed = (SpeedValue > 0.0f) ? SpeedValue : 100.0f;

		UE_LOG(LogTemp, Warning, TEXT("Speed set to: %f"), StoredMoveSpeed);

		// 선택된 오브젝트에 속도 값 적용
		if (SelectedActor)
		{
			if (UMyFlyActorComponent* FlyComponent = SelectedActor->FindComponentByClass<UMyFlyActorComponent>())
			{
				FlyComponent->FlySpeed = StoredMoveSpeed;
				UE_LOG(LogTemp, Warning, TEXT("Speed applied to actor: %f"), FlyComponent->FlySpeed);
			}
		}

		// 속도 입력 후 프리뷰 움직임 시작
		if (PreviewActor)
		{
			GetWorld()->GetTimerManager().SetTimer(PreviewFlyTimer, this, &UFlyWidget::UpdatePreviewMovement, 0.05f, true);
			UE_LOG(LogTemp, Warning, TEXT("Preview movement started."));
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
	FSoftClassPath BP_FunctionObjectClassRef(TEXT("/Game/YWK/BP/BP_MoveandFly.BP_MoveandFly_C"));
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
	FSoftClassPath BP_FunctionObjectClassRef(TEXT("/Game/YWK/BP/BP_MoveandFly.BP_MoveandFly_C"));
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

			// OnSelectionChanged 이벤트 바인딩
			FlyBoxList->OnSelectionChanged.AddDynamic(this, &UFlyWidget::OnFunctionObjectSelected);

			// OnGenerateWidgetEvent 바인딩
			FlyBoxList->OnGenerateWidgetEvent.BindDynamic(this, &UFlyWidget::GenerateComboBoxItem);
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
		FString DisplayName = NewObject->GetName();
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
		// ���ο� ������Ʈ ����
		SelectedActor = AllFunctionObject[SelectedIndex];
		UE_LOG(LogTemp, Warning, TEXT("Selected fly function object: %s"), *SelectedActor->GetName());


		DestroyPreviewActor();

		// ���ο� ������ ���� ����
		SpawnPreviewActor();

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


UWidget* UFlyWidget::GenerateComboBoxItem(FString Item)
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
	 if (!PreviewActor && SelectedActor)
  {
    // BP_PreviewDistance 액터 로드 및 스폰
    UClass* PreviewClass = LoadObject<UClass>(nullptr, TEXT("/Game/YWK/BP/BP_PreviewDistance.BP_PreviewDistance_C"));
    if (PreviewClass)
    {
      // 프리뷰 오브젝트를 SelectedActor 위치에 스폰
      PreviewActor = GetWorld()->SpawnActor<AActor>(PreviewClass, SelectedActor->GetActorLocation(), FRotator::ZeroRotator);
      if (PreviewActor)
      {
        // 초기 위치 설정
        PreviewActor->SetActorLocation(SelectedActor->GetActorLocation()); // 시작 위치로 확실히 설정
        bPreviewDirectionReversed = false; // 초기화: 목표 지점으로 이동하도록 설정
        UE_LOG(LogTemp, Warning, TEXT("PreviewActor spawned at starting location: %s"), *SelectedActor->GetActorLocation().ToString());
      }
    }
  }

// UI에서 속도와 거리 값을 읽어 초기화
  if (FlySpeedText)
  {
    FString SpeedString = FlySpeedText->GetText().ToString();
    StoredMoveSpeed = FCString::Atof(*SpeedString);
  }
  if (FlyHightText)
  {
    FString DistanceString = FlyHightText->GetText().ToString();
    StoredMoveDistance = FCString::Atof(*DistanceString);
  }

// 기본값 설정
  if (StoredMoveSpeed <= 0.0f)
  {
    StoredMoveSpeed = 100.0f; // 기본 속도
  }
  if (StoredMoveDistance <= 0.0f)
  {
    StoredMoveDistance = 1000.0f; // 기본 거리
  }

  UE_LOG(LogTemp, Warning, TEXT("StoredMoveSpeed: %f, StoredMoveDistance: %f"), StoredMoveSpeed, StoredMoveDistance);

// 타이머 시작
  if (PreviewActor)
  {
    GetWorld()->GetTimerManager().SetTimer(PreviewFlyTimer, this, &UFlyWidget::UpdatePreviewMovement, 0.05f, true);
   // UE_LOG(LogTemp, Warning, TEXT("Preview movement timer started."));
  }
}




// 프리뷰 액터의 이동 업데이트
void UFlyWidget::UpdatePreviewMovement()
{
	if (!PreviewActor )
	{
		//PRINTLOG(TEXT("PreviewActor is missing."));
		return;
	}
	else if (!SelectedActor)
	{
	//	PRINTLOG(TEXT("SelectedActor is missing."));
		return;
	}

	// 원본 오브젝트 위치와 목표 지점 계산
	FVector StartLocation = SelectedActor->GetActorLocation(); // 원본 오브젝트 위치
	FVector TargetLocation = StartLocation + (StoredFlyDirection * StoredMoveDistance); // 목표 지점

	FVector CurrentLocation = PreviewActor->GetActorLocation(); // 현재 위치
	FVector Direction = bPreviewDirectionReversed ? -StoredFlyDirection : StoredFlyDirection; // 이동 방향

	// 다음 위치 계산
	FVector NextLocation = CurrentLocation + (Direction * StoredMoveSpeed * 0.05f);

// 	UE_LOG(LogTemp, Warning, TEXT("Start: %s, Target: %s, Current: %s, Next: %s, Direction: %s"),
// 		*StartLocation.ToString(), *TargetLocation.ToString(), *CurrentLocation.ToString(), *NextLocation.ToString(), *Direction.ToString());

	// 목표 지점 도달 여부 확인
	if (!bPreviewDirectionReversed && FVector::Dist(CurrentLocation, TargetLocation) <= 5.0f) // 목표 지점 도달
	{
		bPreviewDirectionReversed = true; // 방향 반전
	//	UE_LOG(LogTemp, Warning, TEXT("PreviewActor reached the target. Reversing direction to START."));
	}
	else if (bPreviewDirectionReversed && FVector::Dist(CurrentLocation, StartLocation) <= 5.0f) // 시작 지점 도달
	{
		bPreviewDirectionReversed = false; // 방향 초기화
		//UE_LOG(LogTemp, Warning, TEXT("PreviewActor reached the start. Reversing direction to TARGET."));
	}

	// 프리뷰 액터 위치 업데이트
	PreviewActor->SetActorLocation(NextLocation);
}


// 프리뷰 위치 업데이트 함수
void UFlyWidget::UpdatePreviewLocation(FVector Direction, float Distance)
{
	// 기존 프리뷰 액터 삭제
	if (PreviewActor)
	{
		PreviewActor->Destroy();
		PreviewActor = nullptr;
	}

	// 새로운 위치 계산 후 프리뷰 액터 생성
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

void UFlyWidget::DestroyPreviewActor()
{
	if (PreviewActor)
	{
		GetWorld()->GetTimerManager().ClearTimer(PreviewFlyTimer); // 타이머 제거
		PreviewActor->Destroy(); // 액터 삭제
		PreviewActor = nullptr;
	}
}