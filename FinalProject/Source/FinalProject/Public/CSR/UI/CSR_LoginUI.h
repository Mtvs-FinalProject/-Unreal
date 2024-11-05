// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "CSR_LoginUI.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UCSR_LoginUI : public UUserWidget
{
	GENERATED_BODY()

virtual void NativeConstruct() override;
    
    // UI 컴포넌트 바인딩
    UPROPERTY(meta = (BindWidget))
    UEditableText* CSR_Login_ID;
    
    UPROPERTY(meta = (BindWidget))
    UEditableText* CSR_Login_Password;
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* CSR_Login_ErrorMessage;
    
    UPROPERTY(meta = (BindWidget))
    UButton* CSR_Login_LoginBTN;
    
    UPROPERTY(meta = (BindWidget))
    UButton* CSR_Login_MembershipBTN;
    
    // 버튼 클릭 이벤트 처리
    UFUNCTION()
    void OnLoginButtonClicked();
    
    UFUNCTION()
    void OnMembershipButtonClicked();
    
    // 로그인 응답 처리
    UFUNCTION()
    void HandleLoginResponse(bool bSuccess, const FString& Message);
    
private:
    // 에러 메시지 표시
    void ShowErrorMessage(const FString& Message);
    
    // 입력 유효성 검사
    bool ValidateInput(FString& ErrorMessage);
};
