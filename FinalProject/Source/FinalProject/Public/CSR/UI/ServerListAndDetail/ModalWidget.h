// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ModalWidget.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UModalWidget : public UUserWidget
{
	GENERATED_BODY()

public:
       // UI 컴포넌트들
    UPROPERTY(meta = (BindWidget))
    class UComboBoxString* SelectMapComboBox;
    
    UPROPERTY(meta = (BindWidget))
    class UCheckBox* CreateRoomCheckBox;
    
    UPROPERTY(meta = (BindWidget))
    class UEditableText* RoomNameInput;

    UPROPERTY(meta = (BindWidget))
    class UButton* CloseButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* CreateButton;

    //// 위젯 바인딩
    //UPROPERTY(meta = (BindWidget))
    //class UTextBlock* TitleText;

    //UPROPERTY(meta = (BindWidget))
    //class UEditableText* RoomNameInput;

protected:
    virtual void NativeConstruct() override;

private:
    // 이벤트 핸들러
    UFUNCTION()
    void OnCreateRoomCheckBoxChanged(bool bIsChecked);

     // 맵 데이터 로드
    UFUNCTION()
    void LoadMapList();
    
    // 맵 리스트 저장
    UPROPERTY()
    TArray<FString> MapList;

    // 닫기 버튼 클릭 이벤트
    UFUNCTION()
    void OnCloseButtonClicked();

    // Create 버튼 클릭 이벤트
    UFUNCTION()
    void OnCreateButtonClicked();
};
