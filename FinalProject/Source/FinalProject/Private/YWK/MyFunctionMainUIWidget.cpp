// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/MyFunctionMainUIWidget.h"
#include "Components/Button.h"

void UMyFunctionMainUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// ������ ��ư
	if (Btn_Edit)
	{
		Btn_Edit->OnClicked.AddDynamic(this, &UMyFunctionMainUIWidget::OnEditButtonClicked);
	}

	// �÷��� ��ư
	if (Btn_Play)
	{
		Btn_Play->OnClicked.AddDynamic(this, &UMyFunctionMainUIWidget::OnPlayButtonClicked);
	}

	// �ݱ� ��ư
	if (Btn_Close)
	{
		Btn_Close->OnClicked.AddDynamic(this, &UMyFunctionMainUIWidget::OnCloseButtonClcked);
	}

	// ó�� ���� false
	bIsEditable = false;
}

// Edit ��ư�� ���� �� ��� ������Ʈ�� UI���� ���� �� �ְ�
void UMyFunctionMainUIWidget::OnEditButtonClicked()
{
	
}

// Play ��ư�� ������ ��� ������Ʈ���� UI�� ������ �ʰ�
void UMyFunctionMainUIWidget::OnPlayButtonClicked()
{

}

// �ش� UI �ݱ�
void UMyFunctionMainUIWidget::OnCloseButtonClcked()
{
	RemoveFromParent();
}
