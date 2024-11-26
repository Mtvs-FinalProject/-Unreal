// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "ModalWidget.h"
#include "ServerListsWidget.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UServerListsWidget : public UUserWidget
{
	GENERATED_BODY()

public:

    virtual void NativeConstruct() override;
    
    virtual void NativeDestruct() override;

    UPROPERTY(meta = (BindWidget))
    UButton* Create_BTN;

    // 모달 위젯 클래스 (Blueprint 설정 가능)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UModalWidget> ModalWidgetClass;

    // 생성된 모달 참조
    UPROPERTY()
    UModalWidget* ActiveModalWidget;

    // "Create" 버튼 클릭 이벤트 핸들러
    UFUNCTION(BlueprintCallable, Category = "ServerList")
    void OnCreateButtonClicked();
};
