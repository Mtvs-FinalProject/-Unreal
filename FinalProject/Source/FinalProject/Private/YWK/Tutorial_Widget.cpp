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
    if (Tutorial_Txt) // 텍스트 블록이 제대로 연결되었는지 확인
    {
        Tutorial_Txt->SetText(FText::FromString(NewText));
        UE_LOG(LogTemp, Warning, TEXT("Widget Text Updated: %s"), *NewText); // 로그 출력
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Tutorial_Txt is null.")); // 텍스트 블록이 null인 경우
    }
}

