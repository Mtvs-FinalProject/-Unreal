// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/Movewidget.h"
#include "Components/Button.h"
#include "YWK/ActionChoice.h"
#include "YWK/MyMoveActorComponent.h"
#include "PSH/PSH_GameMode/PSH_GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/EditableText.h"
#include "Components/ComboBoxString.h"

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
}

// 앞으로 가기
void UMovewidget::OnFrontClicked()
{
	// 컴포넌트 오너 가져오기
	if (SelectedActor)
	{
		UMyMoveActorComponent* MoveComponent = SelectedActor->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			StoredMoveDirection = SelectedActor->GetActorForwardVector();

			// 로그 추가: 앞으로 이동 방향 설정 로그
			UE_LOG(LogTemp, Warning, TEXT("Moving forward. New direction: %s"), *MoveComponent->MoveDirection.ToString());
		}
	}
}

// 뒤로 가기
void UMovewidget::OnBackClicked()
{
	// 컴포넌트 오너 가져오기
	if (SelectedActor)
	{
		UMyMoveActorComponent* MoveComponent = SelectedActor->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			StoredMoveDirection = SelectedActor->GetActorForwardVector() * -1;

			// 로그 추가: 뒤로 이동 방향 설정 로그
			UE_LOG(LogTemp, Warning, TEXT("Moving Backward. New direction: %s"), *MoveComponent->MoveDirection.ToString());
		}
	}
}

// 오른쪽으로 가기
void UMovewidget::OnRightClicked()
{
	// 컴포넌트 오너 가져오기
	if (SelectedActor)
	{
		UMyMoveActorComponent* MoveComponent = SelectedActor->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			StoredMoveDirection = SelectedActor->GetActorRightVector();

			// 로그 추가: 오른쪽으로 이동 방향 설정 로그
			UE_LOG(LogTemp, Warning, TEXT("Moving Right. New direction: %s"), *MoveComponent->MoveDirection.ToString());
		}
	}
}

// 왼쪽으로 가기
void UMovewidget::OnLeftClicked()
{
	// 컴포넌트 오너 가져오기
	if (SelectedActor)
	{
		UMyMoveActorComponent* MoveComponent = SelectedActor->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			StoredMoveDirection = SelectedActor->GetActorRightVector() * -1;

			// 로그 추가: 왼쪽으로 이동 방향 설정 로그
			UE_LOG(LogTemp, Warning, TEXT("Moving Left. New direction: %s"), *MoveComponent->MoveDirection.ToString());
		}
	}
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

// 이 버튼 눌러야 움직이는거 시작됨
void UMovewidget::OnStartButtonClicked()
{
	if (SelectedActor)
	{
		UMyMoveActorComponent* MoveComponent = SelectedActor->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			// 저장된 방향을 MoveDirection으로 설정
			MoveComponent->MoveDirection = StoredMoveDirection;

			// 이동 시작 
			MoveComponent->StartMoving();

			UE_LOG(LogTemp, Warning, TEXT("Starting movement for %s with direction: %s"), *SelectedActor->GetName(), *StoredMoveDirection.ToString());
		}
	}
}

// 이 버튼 누르면 멈춤
void UMovewidget::OnStopButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop button clicked"));

	// 컴포넌트 오너 가져오기
	if (SelectedActor)
	{
		UMyMoveActorComponent* MoveComponent = SelectedActor->FindComponentByClass<UMyMoveActorComponent>();
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

// 최초 자리로 돌아가기
void UMovewidget::OnOriginButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Origin button clicked"));

	// 컴포넌트 오너 가져오기
	if (SelectedActor)
	{
		UMyMoveActorComponent* MoveComponent = SelectedActor->FindComponentByClass<UMyMoveActorComponent>();
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
	// 컴포넌트 가져오기
	if (SelectedActor)
	{
		UMyMoveActorComponent* MoveComponent = SelectedActor->FindComponentByClass<UMyMoveActorComponent>();
		if (MoveComponent)
		{
			// 속도값 설정
			FString SpeedString = SpeedMoveText->GetText().ToString();
			float SpeedValue = FCString::Atof(*SpeedString); // 문자열 float로 바꾸기
			MoveComponent->MoveSpeed = SpeedValue;

			// 거리값 설정
			FString DistnaceString = DistMoveText->GetText().ToString();
			float DistanceValue = FCString::Atof(*DistnaceString); // 이것도 float으로 바꾸기
			MoveComponent->MaxDistance = DistanceValue;

			// UI에 값 업데이트
			UpdateMovementValuesInUI(MoveComponent->MoveSpeed, MoveComponent->MoveDistance);
		}
	}
}


// 속도 입력 필드에서 엔터를 쳤을 때
void UMovewidget::OnSpeedTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		// 입력된 속도 값을 처리
		FString SpeedString = Text.ToString();
		float SpeedValue = FCString::Atof(*SpeedString);

		if (AActor* Owner = GetOwnerFromComponent())
		{
			UMyMoveActorComponent* MoveComponent = Owner->FindComponentByClass<UMyMoveActorComponent>();
			if (MoveComponent)
			{
				MoveComponent->MoveSpeed = SpeedValue;
				// UI에 값 업데이트
				UE_LOG(LogTemp, Warning, TEXT("Speed set to: %f"), SpeedValue);
			}
		}
	}
}

void UMovewidget::OnDistanceTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		// 입력된 거리 값을 처리
		FString DirectString = Text.ToString();
		float DistValues = FCString::Atof(*DirectString);

		if (AActor* Owner = GetOwnerFromComponent())
		{
			UMyMoveActorComponent* MoveComponent = Owner->FindComponentByClass<UMyMoveActorComponent>();
			if (MoveComponent)
			{
				MoveComponent->MaxDistance = DistValues;
				// UI에 값 업데이트
				UE_LOG(LogTemp, Warning, TEXT("Speed set to: %f"), DistValues);
			}
		}
	}
}

AActor* UMovewidget::GetOwnerFromComponent()
{
	// 블루프린트 액터 찾아오기..
	FStringClassReference BP_FunctionObjectClassRef(TEXT("/Game/YWK/BP/BP_MoveandFly.BP_MoveandFly_C"));

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
	// 현재 선택된 `SelectedActor`를 반환
	return SelectedActor;
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
	// ComboBox에서 선택된 인덱스 가져오기
	int32 SelectedIndex = MoveBoxList->FindOptionIndex(SelectedItem);
	if (SelectedIndex != INDEX_NONE && AllFunctionObject.IsValidIndex(SelectedIndex))
	{
		// AllFunctionObject 배열에서 선택된 인덱스의 오브젝트를 가져옴
		SelectedActor = AllFunctionObject[SelectedIndex];
		UE_LOG(LogTemp, Warning, TEXT("Selected function object: %s"), *SelectedActor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No matching function object found for: %s"), *SelectedItem);
	}
}