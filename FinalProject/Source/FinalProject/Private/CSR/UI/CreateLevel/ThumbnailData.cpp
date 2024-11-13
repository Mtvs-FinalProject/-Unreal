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

    // ����� �̹��� ����
    UImage* ThumbnailImage = NewObject<UImage>(Outer);
    ThumbnailImage->SetBrushFromTexture(Texture);

    // �̹����� ���� ���� ���
    float AspectRatio = static_cast<float>(Texture->GetSizeX()) / static_cast<float>(Texture->GetSizeY());

    // ���ϴ� ���̿� ���� �ʺ� ���
    float DesiredWidth = DesiredHeight * AspectRatio;
    ThumbnailImage->SetDesiredSizeOverride(FVector2D(DesiredWidth, DesiredHeight));

    // ����� ��ư ����
    ThumbnailButton = NewObject<UButton>(Outer);

    // ��ư ��Ÿ�� ���� - �е� ����
    FButtonStyle ButtonStyle;
    ButtonStyle.Normal.DrawAs = ESlateBrushDrawType::Box;
    ButtonStyle.Normal.Margin = FMargin(0);
    ThumbnailButton->SetStyle(ButtonStyle);

    ThumbnailButton->AddChild(ThumbnailImage);

    return ThumbnailButton;
}