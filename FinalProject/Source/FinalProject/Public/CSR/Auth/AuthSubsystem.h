// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Http.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Delegates/DelegateCombinations.h"
#include "AuthSubsystem.generated.h"

/**
 * 
 */

 // 응답 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSignUpResponse, bool, bSuccess, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoginResponse, bool, bSuccess, const FString&, Message);

UCLASS()
class FINALPROJECT_API UAuthSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    // 초기화
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    
    // 회원가입 요청
    UFUNCTION(BlueprintCallable, Category = "Authentication")
    void RequestSignUp(const FString& UserID, const FString& Password, const FString& Nickname);
    
    // 로그인 요청
    UFUNCTION(BlueprintCallable, Category = "Authentication")
    void RequestLogin(const FString& UserID, const FString& Password);
    
    // 토큰 관리
    UFUNCTION(BlueprintCallable, Category = "Authentication")
    FString GetAuthToken() const { return AuthToken; }
    
    // 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "Authentication")
    FOnSignUpResponse OnSignUpResponse;
    
    UPROPERTY(BlueprintAssignable, Category = "Authentication")
    FOnLoginResponse OnLoginResponse;
    
private:
    // HTTP 모듈
    FHttpModule* HttpModule;
    
    // 인증 토큰
    FString AuthToken;
    
    // 서버 URL
    const FString AuthServerUrl = TEXT("http://www.mtvs.store:1818");

    const FString AuthServerJoin = TEXT("/api/v1/users/signup");

    const FString AuthServerLogin = TEXT("/api/v1/users/login");
    
    // 응답 처리 함수
    void OnSignUpRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
    void OnLoginRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
    
    // HTTP 요청 생성 헬퍼 함수
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> CreateHttpRequest(const FString& Endpoint);
	
};
