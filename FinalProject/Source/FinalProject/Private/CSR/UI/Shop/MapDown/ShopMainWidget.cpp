// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/Shop/MapDown/ShopMainWidget.h"

void UShopMainWidget::NativeConstruct()
{
	if (MapDownBTN)
    {
        MapDownBTN->OnClicked.AddDynamic(this, &UShopMainWidget::OnClickMapDownBTN);
    }

	if (AvatarBTN)
    {
        AvatarBTN->OnClicked.AddDynamic(this, &UShopMainWidget::OnClickAvatarBTN);
    }

}

void UShopMainWidget::OnClickMapDownBTN()
{
    if (ShopSwitch)
    {
        this->ShopSwitch->SetActiveWidgetIndex(0);
    }
    if (MapDownBTN)
    {
        // 버튼 비활성화 또는 시각적 피드백 추가
        MapDownBTN->SetIsEnabled(false);
    }

    if (AvatarBTN)
    {
        // 다른 버튼 활성화
        AvatarBTN->SetIsEnabled(true);
    }
}

void UShopMainWidget::OnClickAvatarBTN()
{
    if (ShopSwitch)
    {
        this->ShopSwitch->SetActiveWidgetIndex(1);
    }
    if (MapDownBTN)
    {
        // 버튼 비활성화 또는 시각적 피드백 추가
        MapDownBTN->SetIsEnabled(true);
    }

    if (AvatarBTN)
    {
        // 다른 버튼 활성화
        AvatarBTN->SetIsEnabled(false);
    }
}
