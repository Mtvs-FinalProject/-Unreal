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

	// 게임모드에서 오브젝트 생성 이벤트를 바인딩
	APSH_GameModeBase* GameMode = Cast<APSH_GameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->OnObjectCreated.AddDynamic(this, &UMovewidget::AddObjectToComboBox);
	}

	// 뒤로가기 버튼
	if (Btn_MoveBack)
	{
		Btn_MoveBack->OnClicked.AddDynamic(this, &UMovewidget::OnMoveBackClicked);
	}
	// 앞으로 가기 버튼
	if (Btn_Front)
	{
		Btn_Front->OnClicked.AddDynamic(this, &UMovewidget::OnFrontClicked);
	}

	// 뒤로 가기 버튼
	if (Btn_Back)
	{
		Btn_Back->OnClicked.AddDynamic(this, &UMovewidget::OnBackClicked);
	}

	// 왼쪽으로 가기 버튼
	if (Btn_Left)
	{
		Btn_Left->OnClicked.AddDynamic(this, &UMovewidget::OnLeftClicked);
	}

	// 오른쪽으로 가기 버튼
	if (Btn_Right)
	{
		Btn_Right->OnClicked.AddDynamic(this, &UMovewidget::OnRightClicked);
	}

	// 기능 시작 버튼
	if (Btn_MoveStart)
	{
		Btn_MoveStart->OnClicked.AddDynamic(this, &UMovewidget::OnStartButtonClicked);
	}

	// 기능 정지 버튼
	if (Btn_MoveStop)
	{
		Btn_MoveStop->OnClicked.AddDynamic(this, &UMovewidget::OnStopButtonClicked);
	}

	// 최초 자리로 돌아가기
	if (Btn_MoveOrigin)
	{
		Btn_MoveOrigin->OnClicked.AddDynamic(this, &UMovewidget::OnOriginButtonClicked);
	}

	// 속도 입력 필드에 OnTextCommitted 이벤트 바인딩
	if (SpeedMoveText)
	{
		SpeedMoveText->OnTextCommitted.AddDynamic(this, &UMovewidget::OnSpeedTextCommitted);
	}

	// 거리 입력 필드에 OnTextCommitted 이벤트 바인딩
	if (DistMoveText)
	{
		DistMoveText->OnTextCommitted.AddDynamic(this, &UMovewidget::OnDistanceTextCommitted);
	}

	InitializeFunctionObjects();

	// 왕복 모드 체크박스와 횟수 입력 필드 이벤트 바인딩
	if (Chk_LoopMode)
	{
		Chk_LoopMode->OnCheckStateChanged.AddDynamic(this, &UMovewidget::OnLoopModeCheckChanged);
	}

	if (Chk_SingleDirectionMode)
	{
		Chk_SingleDirectionMode->OnCheckStateChanged.AddDynamic(this, &UMovewidget::OnSingleDirectionCheckChanged);
	}
}

// 앞으로 가기
void UMovewidget::OnFrontClicked()
{
	SetMoveDirection(FVector(1.0f, 0.0f, 0.0f));  // 전방 벡터
}

// 뒤로 가기
void UMovewidget::OnBackClicked()
{
	SetMoveDirection(FVector(-1.0f, 0.0f, 0.0f));  // 후방 벡터
}

// 오른쪽으로 가기
void UMovewidget::OnRightClicked()
{
	SetMoveDirection(FVector(0.0f, 1.0f, 0.0f));  // 오른쪽 벡터
}

// 왼쪽으로 가기
void UMovewidget::OnLeftClicked()
{
	SetMoveDirection(FVector(0.0f, -1.0f, 0.0f));  // 왼쪽 벡터
}

// 현재 띄운 ui 지우고 뒤에 ui 열기
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

// 기능 시작 버튼
void UMovewidget::OnStartButtonClicked()
{
	// Start 버튼을 누를 때 미리보기 오브젝트가 사라지도록 설정
	if (PreviewActor)
	{
		PreviewActor->SetActorHiddenInGame(true);
		PreviewActor = nullptr;
	}
	// 단일 SelectedActor 처리
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

	// 다중 컴포넌트 처리
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



// 이 버튼 누르면 멈춤
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

// 최초 자리로 돌아가기
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



// 속도와 거리 값 set text
void UMovewidget::UpdateMovementValuesInUI(float SpeedValue, float DistanceValue)
{
	// 속도 값을 UI의 EditableText에 설정
	if (SpeedMoveText)
	{
		FText SpeedText = FText::AsNumber(SpeedValue); // 숫자를 FText로 변환
		SpeedMoveText->SetText(SpeedText);
	}
	// 거리 값을 UI의 EditableText에 설정
	if (DistMoveText)
	{
		FText DistText = FText::AsNumber(DistanceValue);
		DistMoveText->SetText(DistText);
	}
}

// 속도나 거리 값 넣는 함수
void UMovewidget::ApplyMovementValues()
{
	if (SelectedActor)
	{
		UMyMoveActorComponent* MoveComponent = SelectedActor->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			// 속도 값 넣는 부분
			FString SpeedString = SpeedMoveText->GetText().ToString();
			float SpeedValue = FCString::Atof(*SpeedString);
			MoveSpeedMap.FindOrAdd(MoveComponent) = SpeedValue;

			// 거리 값 넣는 부분
			FString DistanceString = DistMoveText->GetText().ToString();
			float DistanceValue = FCString::Atof(*DistanceString);
			MoveComponent->MaxDistance = DistanceValue;

			UpdateMovementValuesInUI(SpeedValue, DistanceValue);
			UE_LOG(LogTemp, Warning, TEXT("Movement values applied for %s with speed: %f, distance: %f"),
				*MoveComponent->GetOwner()->GetName(), SpeedValue, DistanceValue);
		}
	}
}


// 속도 입력 필드에서 엔터를 쳤을 때
void UMovewidget::OnSpeedTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{	//  속도 입력 값 처리
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
		// 입력된 거리 값 처리
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
	// SelectedActor가 설정되어 있으면 바로 반환
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
			// 첫 번째 발견된 액터를 기본으로 설정
			SelectedActor = FoundActors[0];
			return SelectedActor;
		}
		// 없으면 새로 생성
		SelectedActor = GetWorld()->SpawnActor<AActor>(BP_FunctionObjectClass);
		return SelectedActor;
	}
	return nullptr;
}


// 기능 오브젝트 초기화
void UMovewidget::InitializeFunctionObjects()
{
	FStringClassReference BP_FunctionObjectClassRef(TEXT("/Game/YWK/BP/BP_MoveandFly.BP_MoveandFly_C"));
	UClass* BP_FunctionObjectClass = BP_FunctionObjectClassRef.TryLoadClass<AActor>();

	if (BP_FunctionObjectClass && GetWorld())
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), BP_FunctionObjectClass, AllFunctionObject);

		// ComboBox에 초기 오브젝트 추가
		if (MoveBoxList)
		{
			for (AActor* FunctionObject : AllFunctionObject)
			{
				if (FunctionObject)
				{
					MoveBoxList->AddOption(FunctionObject->GetName());
				}
			}
			// 콤보박스 선택 변경 시 이벤트 바인딩
			MoveBoxList->OnSelectionChanged.AddDynamic(this, &UMovewidget::OnFunctionObjectSelected);
		}
		// 배열의 첫 번째 오브젝트를 기본 선택
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
		// ComboBox에 오브젝트 이름 추가
		FString DisplayName = NewObject->GetActorLabel(); // 에디터에서 설정한 이름
		MoveBoxList->AddOption(DisplayName);

		// AllFunctionObject 배열에 오브젝트 참조 추가하여 ComboBox와 인덱스를 맞추기
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
			// UI에 선택된 MoveComponent의 값을 업데이트
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

// 다중 MoveComponent 추가 함수
void UMovewidget::AddControlledMoveComponent(UMyMoveActorComponent* MoveComponent)
{
	if (MoveComponent && !ControlledMoveComponents.Contains(MoveComponent))
	{
		ControlledMoveComponents.Add(MoveComponent);
	}
}

// 방향정하기
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

void UMovewidget::SpawnPreviewActor()
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

void UMovewidget::UpdatePreviewLocation(FVector Direction, float Distance)
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