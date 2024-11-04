// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/AuthUI/MembershipJoinWidget.h"
#include "Kismet/GameplayStatics.h"
#include "CSR/Auth/AuthSubsystem.h"
#include "CSR/GameModes/Login/LoginGameMode.h"

void UMembershipJoinWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 버튼 클릭 이벤트 바인딩
    if (CSR_Join_JoinBTN)
    {
        CSR_Join_JoinBTN->OnClicked.AddDynamic(this, &UMembershipJoinWidget::OnJoinButtonClicked);
    }

    if (CSR_Join_ExitBTN)
    {
        CSR_Join_ExitBTN->OnClicked.AddDynamic(this, &UMembershipJoinWidget::OnExitButtonClicked);
    }

    // 에러 메시지 초기화
    if (CSR_Join_ErrorMessage)
    {
        CSR_Join_ErrorMessage->SetText(FText::GetEmpty());
    }

    // AuthSubsystem 이벤트 바인딩
    if (UAuthSubsystem* AuthSubsystem = GetGameInstance()->GetSubsystem<UAuthSubsystem>())
    {
        AuthSubsystem->OnSignUpResponse.AddDynamic(this, &UMembershipJoinWidget::HandleSignUpResponse);
    }
}

void UMembershipJoinWidget::OnJoinButtonClicked()
{
    // 처음 문자열 초기화
    ShowErrorMessage(TEXT(""));

    // 입력 유효성 검사
    FString ErrorMessage;
    if (!ValidateInput(ErrorMessage))
    {
        ShowErrorMessage(ErrorMessage);
        return;
    }

    // 회원가입 요청
    if (UAuthSubsystem* AuthSubsystem = GetGameInstance()->GetSubsystem<UAuthSubsystem>())
    {
        FString UserID = CSR_Join_ID->GetText().ToString();
        FString Password = CSR_Join_Password->GetText().ToString();
        FString Nickname = CSR_Join_Nickname->GetText().ToString();

        // 버튼 비활성화 (응답이 올 때까지)
        SetButtonsEnabled(false);

        // 회원가입 요청 전송
        AuthSubsystem->RequestSignUp(UserID, Password, Nickname);
    }
}

void UMembershipJoinWidget::OnExitButtonClicked()
{
    // 로그인 화면으로 돌아가기
    if (ALoginGameMode* LoginGameMode = Cast<ALoginGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        LoginGameMode->ShowLoginUI();
    }
}

void UMembershipJoinWidget::HandleSignUpResponse(bool bSuccess, const FString& Message)
{
    // 버튼 재활성화
    SetButtonsEnabled(true);

    if (bSuccess)
    {
        // 회원가입 성공 시 로그인 화면으로 이동
        if (ALoginGameMode* LoginGameMode = Cast<ALoginGameMode>(UGameplayStatics::GetGameMode(this)))
        {
            LoginGameMode->ShowLoginUI();
        }
    }
    else
    {
        ShowErrorMessage(Message);
    }
}

void UMembershipJoinWidget::ShowErrorMessage(const FString& Message)
{
    if (CSR_Join_ErrorMessage)
    {
        CSR_Join_ErrorMessage->SetText(FText::FromString(Message));
    }
}

bool UMembershipJoinWidget::ValidateInput(FString& ErrorMessage)
{
    if (!CSR_Join_ID || !CSR_Join_Password || !CSR_Join_Nickname)
    {
        ErrorMessage = TEXT("Internal UI Error");
        return false;
    }

    FString UserID = CSR_Join_ID->GetText().ToString();
    FString Password = CSR_Join_Password->GetText().ToString();
    FString Nickname = CSR_Join_Nickname->GetText().ToString();

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

    if (Nickname.IsEmpty())
    {
        ErrorMessage = TEXT("닉네임을 입력해주세요.");
        return false;
    }

    return true;
}

void UMembershipJoinWidget::SetButtonsEnabled(bool bEnabled)
{
    if (CSR_Join_JoinBTN)
    {
        CSR_Join_JoinBTN->SetIsEnabled(bEnabled);
    }

    if (CSR_Join_ExitBTN)
    {
        CSR_Join_ExitBTN->SetIsEnabled(bEnabled);
    }
}