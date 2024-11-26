// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/Tutorial_Widget.h"
#include "Components/Button.h"
#include "YWK/TutorialBot.h"

void UTutorial_Widget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UTutorial_Widget::UpdateText(const FString& NewText)
{
    if (Tutorial_Txt) // �ؽ�Ʈ ����� ����� ����Ǿ����� Ȯ��
    {
        Tutorial_Txt->SetText(FText::FromString(NewText));
        UE_LOG(LogTemp, Warning, TEXT("Widget Text Updated: %s"), *NewText); // �α� ���
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Tutorial_Txt is null.")); // �ؽ�Ʈ ����� null�� ���
    }
}

