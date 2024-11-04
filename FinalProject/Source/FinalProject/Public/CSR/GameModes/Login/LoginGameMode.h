// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LoginGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API ALoginGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
    ALoginGameMode();
    
    virtual void BeginPlay() override;
    
    // UI 전환 함수
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowLoginUI();
    
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowSignUpUI();
    
    // 현재 표시된 위젯 가져오기
    UFUNCTION(BlueprintPure, Category = "UI")
    UUserWidget* GetCurrentWidget() const { return CurrentWidget; }
    
protected:
    // 위젯 클래스 레퍼런스
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> LoginWidgetClass;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> SignUpWidgetClass;
    
private:
    // 현재 표시 중인 위젯
    UPROPERTY()
    class UUserWidget* CurrentWidget;
    
    // 위젯 전환 헬퍼 함수
    void SwitchUIWidget(TSubclassOf<UUserWidget> NewWidgetClass);
    
    // 인증 응답 처리
    UFUNCTION()
    void HandleLoginResponse(bool bSuccess, const FString& Message);
    
    UFUNCTION()
    void HandleSignUpResponse(bool bSuccess, const FString& Message);
};
