// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/GameModes/Login/LoginGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "CSR/Auth/AuthSubsystem.h"

ALoginGameMode::ALoginGameMode()
{
	// 위젯 블루프린트 클래스 설정
	static ConstructorHelpers::FClassFinder<UUserWidget> LoginWidgetBPClass(TEXT("/Game/CSR/UI/Login/WBP_Login"));
	if (LoginWidgetBPClass.Succeeded())
	{
		LoginWidgetClass = LoginWidgetBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> SignUpWidgetBPClass(TEXT("/Game/CSR/UI/Login/WBP_MembershipJoin"));
	if (SignUpWidgetBPClass.Succeeded())
	{
		SignUpWidgetClass = SignUpWidgetBPClass.Class;
	}
}

void ALoginGameMode::BeginPlay()
{
    Super::BeginPlay();

    // 인증 서브시스템 이벤트 바인딩
    if (UAuthSubsystem* AuthSubsystem = GetGameInstance()->GetSubsystem<UAuthSubsystem>())
    {
        AuthSubsystem->OnLoginResponse.AddDynamic(this, &ALoginGameMode::HandleLoginResponse);
        AuthSubsystem->OnSignUpResponse.AddDynamic(this, &ALoginGameMode::HandleSignUpResponse);
    }

    // 초기 UI 표시
    ShowLoginUI();

    // 입력 모드를 UI 전용으로 설정
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (PlayerController)
    {
        PlayerController->SetInputMode(FInputModeUIOnly());
        PlayerController->bShowMouseCursor = true;
    }
}

void ALoginGameMode::ShowLoginUI()
{
    SwitchUIWidget(LoginWidgetClass);
}

void ALoginGameMode::ShowSignUpUI()
{
    SwitchUIWidget(SignUpWidgetClass);
}

void ALoginGameMode::SwitchUIWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
    if (CurrentWidget)
    {
        CurrentWidget->RemoveFromParent();
        CurrentWidget = nullptr;
    }

    if (NewWidgetClass)
    {
        CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
        if (CurrentWidget)
        {
            CurrentWidget->AddToViewport();
        }
    }
}

void ALoginGameMode::HandleLoginResponse(bool bSuccess, const FString& Message)
{
    // 로그인 성공 시 처리
    if (bSuccess)
    {
        // TODO: 데디케이티드 서버 접속 로직 구현
        UE_LOG(LogTemp, Log, TEXT("TMT Login Successful: %s"), *Message);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("TMT Login Failed: %s"), *Message);
        // UI에 에러 메시지 표시 로직 추가 가능
    }
}

void ALoginGameMode::HandleSignUpResponse(bool bSuccess, const FString& Message)
{
    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("TMT SignUp Successful: %s"), *Message);
        // 회원가입 성공 시 로그인 화면으로 전환
        ShowLoginUI();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("TMT SignUp Failed: %s"), *Message);
        // UI에 에러 메시지 표시 로직 추가 가능
    }
}
