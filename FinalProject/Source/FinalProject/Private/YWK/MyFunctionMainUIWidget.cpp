// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/MyFunctionMainUIWidget.h"
#include "Components/Button.h"

void UMyFunctionMainUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 에디터 버튼
	if (Btn_Edit)
	{
		Btn_Edit->OnClicked.AddDynamic(this, &UMyFunctionMainUIWidget::OnEditButtonClicked);
	}

	// 플레이 버튼
	if (Btn_Play)
	{
		Btn_Play->OnClicked.AddDynamic(this, &UMyFunctionMainUIWidget::OnPlayButtonClicked);
	}

	// 닫기 버튼
	if (Btn_Close)
	{
		Btn_Close->OnClicked.AddDynamic(this, &UMyFunctionMainUIWidget::OnCloseButtonClcked);
	}

	// 처음 상태 false
	bIsEditable = false;
}

// Edit 버튼을 누를 때 기능 오브젝트의 UI들이 나올 수 있게
void UMyFunctionMainUIWidget::OnEditButtonClicked()
{
	
}

// Play 버튼을 누르면 기능 오브젝트들의 UI를 나오지 않게
void UMyFunctionMainUIWidget::OnPlayButtonClicked()
{

}

// 해당 UI 닫기
void UMyFunctionMainUIWidget::OnCloseButtonClcked()
{
	RemoveFromParent();
}
