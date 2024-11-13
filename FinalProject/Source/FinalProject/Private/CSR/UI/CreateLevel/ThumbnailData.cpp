// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/CreateLevel/ThumbnailData.h"
#include "Components/Image.h"

UThumbnailData::UThumbnailData()
    : ThumbnailButton(nullptr)
    , Texture(nullptr)
    , ThumbnailIndex(-1)
{
}

void UThumbnailData::Initialize(UTexture2D* InTexture, const FString& InImagePath)
{
    Texture = InTexture;
    ImagePath = InImagePath;
}

UButton* UThumbnailData::CreateThumbnailButton(UObject* Outer, float DesiredHeight)
{
    if (!Texture) return nullptr;

    // 썸네일 이미지 생성
    UImage* ThumbnailImage = NewObject<UImage>(Outer);
    ThumbnailImage->SetBrushFromTexture(Texture);

    // 이미지의 원래 비율 계산
    float AspectRatio = static_cast<float>(Texture->GetSizeX()) / static_cast<float>(Texture->GetSizeY());

    // 원하는 높이에 맞춰 너비 계산
    float DesiredWidth = DesiredHeight * AspectRatio;
    ThumbnailImage->SetDesiredSizeOverride(FVector2D(DesiredWidth, DesiredHeight));

    // 썸네일 버튼 생성
    ThumbnailButton = NewObject<UButton>(Outer);

    // 버튼 스타일 설정 - 패딩 제거
    FButtonStyle ButtonStyle;
    ButtonStyle.Normal.DrawAs = ESlateBrushDrawType::Box;
    ButtonStyle.Normal.Margin = FMargin(0);
    ThumbnailButton->SetStyle(ButtonStyle);

    ThumbnailButton->AddChild(ThumbnailImage);

    return ThumbnailButton;
}