// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/Main/MainPageWidget.h"

void UMainPageWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 버튼 클릭 이벤트 바인딩
    if (ServerListBTN)
    {
        ServerListBTN->OnClicked.AddDynamic(this, &UMainPageWidget::OnClickServerListBTN);
    }

    if (ShopBTN)
    {
        ShopBTN->OnClicked.AddDynamic(this, &UMainPageWidget::OnClickShopBTN);
    }
    OnClickServerListBTN();
}

void UMainPageWidget::OnClickServerListBTN()
{
    if (SwitchWidget)
    {
        // WidgetSwitcher의 활성 위젯을 서버 리스트 페이지(Index 0)로 설정
        SwitchWidget->SetActiveWidgetIndex(0);
    }

    if (ServerListBTN)
    {
        // 버튼 비활성화 또는 시각적 피드백 추가
        ServerListBTN->SetIsEnabled(false);
    }

    if (ShopBTN)
    {
        // 다른 버튼 활성화
        ShopBTN->SetIsEnabled(true);
    }
}

void UMainPageWidget::OnClickShopBTN()
{
    if (SwitchWidget)
    {
        // WidgetSwitcher의 활성 위젯을 상점 페이지(Index 1)로 설정
        SwitchWidget->SetActiveWidgetIndex(1);
    }

    if (ShopBTN)
    {
        // 버튼 비활성화 또는 시각적 피드백 추가
        ShopBTN->SetIsEnabled(false);
    }

    if (ServerListBTN)
    {
        // 다른 버튼 활성화
        ServerListBTN->SetIsEnabled(true);
    }
}

