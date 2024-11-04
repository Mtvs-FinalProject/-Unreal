// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "MembershipJoinWidget.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UMembershipJoinWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    
    // UI 컴포넌트 바인딩
    UPROPERTY(meta = (BindWidget))
    UEditableText* CSR_Join_ID;
    
    UPROPERTY(meta = (BindWidget))
    UEditableText* CSR_Join_Password;
    
    UPROPERTY(meta = (BindWidget))
    UEditableText* CSR_Join_Nickname;
    
    UPROPERTY(meta = (BindWidget))
    UButton* CSR_Join_JoinBTN;
    
    UPROPERTY(meta = (BindWidget))
    UButton* CSR_Join_ExitBTN;
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* CSR_Join_ErrorMessage;

    // 버튼 클릭 이벤트 처리
    UFUNCTION()
    void OnJoinButtonClicked();
    
    UFUNCTION()
    void OnExitButtonClicked();
    
    UFUNCTION()
    void ShowErrorMessage(const FString& Message);

    // 회원가입 응답 처리
    UFUNCTION()
    void HandleSignUpResponse(bool bSuccess, const FString& Message);
    
private:
    // 입력 유효성 검사
    bool ValidateInput(FString& ErrorMessage);
    
    // 버튼 활성화/비활성화 제어
    void SetButtonsEnabled(bool bEnabled);
	
};
