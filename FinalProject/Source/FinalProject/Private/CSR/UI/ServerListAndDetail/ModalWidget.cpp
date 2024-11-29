// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/ServerListAndDetail/ModalWidget.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "PSH/PSH_Player/PSH_PlayerController.h"

void UModalWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 컴포넌트 유효성 검사 및 이벤트 바인딩
    if (CreateRoomCheckBox)
    {
        CreateRoomCheckBox->OnCheckStateChanged.AddDynamic(this, &UModalWidget::OnCreateRoomCheckBoxChanged);
    }

    if (CloseButton)
    {
        CloseButton->OnClicked.AddDynamic(this, &UModalWidget::OnCloseButtonClicked);
    }

    if (CreateButton)
    {
        CreateButton->OnClicked.AddDynamic(this, &UModalWidget::OnCreateButtonClicked);
    }

    // 맵 리스트 로드
    LoadMapList();
}

void UModalWidget::OnCreateRoomCheckBoxChanged(bool bIsChecked)
{
    if (SelectMapComboBox)
    {
        // CreateRoom 체크박스 상태에 따라 SelectMap 활성화/비활성화
        SelectMapComboBox->SetIsEnabled(!bIsChecked);
    }
}

void UModalWidget::LoadMapList()
{
    if (!SelectMapComboBox)
    {
        return;
    }

    // Saved/MapData 경로 설정
    FString MapPath = FPaths::ProjectSavedDir() / TEXT("MapData");
    TArray<FString> Files;

    // .json 파일 검색
    IFileManager::Get().FindFiles(Files, *MapPath, TEXT("*.json"));

    // ComboBox 초기화
    SelectMapComboBox->ClearOptions();
    MapList.Empty();

    // 찾은 파일들을 ComboBox에 추가
    for (const FString& File : Files)
    {
        FString MapName = FPaths::GetBaseFilename(File);
        SelectMapComboBox->AddOption(MapName);
        MapList.Add(MapName);
    }
}

FString UModalWidget::LoadJsonFromFile(const FString& MapName)
{
    FString JsonString;
    FString FilePath = FPaths::ProjectSavedDir() / TEXT("MapData") / (MapName + TEXT(".json"));
    UE_LOG(LogTemp, Warning, TEXT("csr %s"), *MapName);

    if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("csr Failed to load JSON file: %s"), *FilePath);
        return TEXT("");
    }
    UE_LOG(LogTemp, Warning, TEXT("csr csr sss %s"), *JsonString);
    return JsonString;
}

void UModalWidget::OnCloseButtonClicked()
{

    // 입력 모드 원래대로 복원
    if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        PlayerController->SetInputMode(FInputModeGameAndUI());
        PlayerController->bShowMouseCursor = true;
    }

    // 모달 제거
    RemoveFromParent();

}

void UModalWidget::OnCreateButtonClicked()
{
    FString RoomName = RoomNameInput->GetText().ToString();
    bool bIsCreateRoom = CreateRoomCheckBox->IsChecked();
    FString SelectedMap = SelectMapComboBox->GetSelectedOption();

    UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("csr 방 이름: %s"), *RoomName));
    UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("csr 새로운 맵 생성: %s"), bIsCreateRoom ? TEXT("Yes") : TEXT("No")));
    UE_LOG(LogTemp, Log, TEXT("%s"), *FString::Printf(TEXT("csr 선택된 맵: %s"), *SelectedMap));

    // 입력 검증
    FString ErrorMessage;
    if (!ValidateInputs(RoomName, SelectedMap, ErrorMessage))
    {
        ShowErrorMessage(ErrorMessage);
        return;
    }

    // PlayerController 가져오기
    if (APSH_PlayerController* PC = Cast<APSH_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
    {
        if (bIsCreateRoom)
        {
            // Create 모드 - 빈 방 생성
            PC->ServerRequestCreateAutoRoom(RoomName);
        }
        else
        {
            // Play 모드 - JSON 데이터로 방 생성

            FString JsonString = LoadJsonFromFile(SelectedMap);
			if (!JsonString.IsEmpty())
			{
                JsonString = TEXT("");
				PC->ServerRequestCreateAutoRoom(RoomName, SelectedMap, JsonString);
			}
			else
			{

				ShowErrorMessage(FString::Printf(TEXT("Failed to load map data: %s"), *SelectedMap));

			}
        }
    }

    // 모달 닫기
    OnCloseButtonClicked();
}

bool UModalWidget::ValidateInputs(const FString& RoomName, const FString& SelectedMap, FString& OutErrorMessage)
{
    if (RoomName.IsEmpty())
    {
        OutErrorMessage = TEXT("Room name cannot be empty");
        return false;
    }

    if (!CreateRoomCheckBox->IsChecked() && SelectedMap.IsEmpty())
    {
        OutErrorMessage = TEXT("Please select a map");
        return false;
    }

    return true;
}

void UModalWidget::ShowErrorMessage(const FString& Message)
{
    // 여기에 에러 메시지를 표시하는 UI 로직 구현
        // 예: 메시지 박스 표시
    UE_LOG(LogTemp, Warning, TEXT("Modal Error: %s"), *Message);
}
