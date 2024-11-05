// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/CSR_LoginUI.h"
#include "Kismet/GameplayStatics.h"
#include "CSR/Auth/AuthSubsystem.h"
#include "CSR/GameModes/Login/LoginGameMode.h"

void UCSR_LoginUI::NativeConstruct()
{
    Super::NativeConstruct();

    // 버튼 클릭 이벤트 바인딩
    if (CSR_Login_LoginBTN)
    {
        CSR_Login_LoginBTN->OnClicked.AddDynamic(this, &UCSR_LoginUI::OnLoginButtonClicked);
    }

    if (CSR_Login_MembershipBTN)
    {
        CSR_Login_MembershipBTN->OnClicked.AddDynamic(this, &UCSR_LoginUI::OnMembershipButtonClicked);
    }

    // 에러 메시지 초기화
    if (CSR_Login_ErrorMessage)
    {
        CSR_Login_ErrorMessage->SetText(FText::GetEmpty());
    }

    // AuthSubsystem 이벤트 바인딩
    if (UAuthSubsystem* AuthSubsystem = GetGameInstance()->GetSubsystem<UAuthSubsystem>())
    {
        AuthSubsystem->OnLoginResponse.AddDynamic(this, &UCSR_LoginUI::HandleLoginResponse);
    }
}

// 로그인 요청 버튼
void UCSR_LoginUI::OnLoginButtonClicked()
{
    // 에러 메시지 초기화
    ShowErrorMessage(TEXT(""));

    // 입력 유효성 검사
    FString ErrorMessage;
    if (!ValidateInput(ErrorMessage))
    {
        ShowErrorMessage(ErrorMessage);
        return;
    }

    // 로그인 요청
    if (UAuthSubsystem* AuthSubsystem = GetGameInstance()->GetSubsystem<UAuthSubsystem>())
    {
        FString UserID = CSR_Login_ID->GetText().ToString();
        FString Password = CSR_Login_Password->GetText().ToString();

        AuthSubsystem->RequestLogin(UserID, Password);

        // 로그인 버튼 비활성화 (응답이 올 때까지)
        CSR_Login_LoginBTN->SetIsEnabled(false);
    }
}

// 회원가입으로 이동
void UCSR_LoginUI::OnMembershipButtonClicked()
{
    // LoginGameMode를 통해 회원가입 UI로 전환
    if (ALoginGameMode* LoginGameMode = Cast<ALoginGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        LoginGameMode->ShowSignUpUI();
    }
}

void UCSR_LoginUI::HandleLoginResponse(bool bSuccess, const FString& Message)
{
    // 로그인 버튼 재활성화
    CSR_Login_LoginBTN->SetIsEnabled(true);

    if (!bSuccess)
    {
        ShowErrorMessage(Message);
        return;
    }

    // 로그인 성공 시의 처리는 LoginGameMode에서 수행
}

void UCSR_LoginUI::ShowErrorMessage(const FString& Message)
{
    if (CSR_Login_ErrorMessage)
    {
        CSR_Login_ErrorMessage->SetText(FText::FromString(Message));
    }
}

bool UCSR_LoginUI::ValidateInput(FString& ErrorMessage)
{
    if (!CSR_Login_ID || !CSR_Login_Password)
    {
        ErrorMessage = TEXT("Internal UI Error");
        return false;
    }

    FString UserID = CSR_Login_ID->GetText().ToString();
    FString Password = CSR_Login_Password->GetText().ToString();

    if (UserID.IsEmpty())
    {
        ErrorMessage = TEXT("아이디를 입력해주세요.");
        return false;
    }

    if (Password.IsEmpty())
    {
        ErrorMessage = TEXT("비밀번호를 입력해주세요.");
        return false;
    }

    return true;
}