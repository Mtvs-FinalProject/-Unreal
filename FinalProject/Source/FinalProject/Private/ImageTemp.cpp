// Fill out your copyright notice in the Description page of Project Settings.


#include "ImageTemp.h"
#include "ImageUtils.h"
#include "CSR/UI/CreateLevel/ImageManagerWidget.h"

void UImageTemp::NativeConstruct()
{
    AddImageButton->OnClicked.AddDynamic(this, &UImageTemp::OnAddImageButtonClicked);
}

void UImageTemp::OnAddImageButtonClicked()
{
    OpenImagePicker();
}

TArray<FString> UImageTemp::GetImageList() const
{
    TArray<FString> ImgList;
    UE_LOG(LogTemp, Warning, TEXT("csr %s, %d"), *LastSelectedFilePath, ImgList.Num());
    ImgList.Add(LastSelectedFilePath);
    return (ImgList);
}

void UImageTemp::OpenImagePicker()
{
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (DesktopPlatform)
    {
        TArray<FString> OutFilenames;
        const FString FileTypes = TEXT("Image Files (*.png;*.jpg;*.jpeg)|*.png;*.jpg;*.jpeg");
        const FString DefaultPath = LastSelectedFilePath.IsEmpty() ? FPaths::ProjectContentDir() : FPaths::GetPath(LastSelectedFilePath);

        const bool bOpened = DesktopPlatform->OpenFileDialog(
            nullptr,
            TEXT("이미지 선택"),
            DefaultPath,
            TEXT(""),
            FileTypes,
            EFileDialogFlags::Multiple,
            OutFilenames
        );

        if (bOpened && OutFilenames.Num() > 0)
        {
            LastSelectedFilePath = OutFilenames[0];

            for (const FString& ImagePath : OutFilenames)
            {
                UTexture2D* LoadedTexture = FImageUtils::ImportFileAsTexture2D(ImagePath);
                UpdatePreviewImage(LoadedTexture);
            }

           
        }
    }
}

void UImageTemp::UpdatePreviewImage(UTexture2D* Img)
{
    if (Img)
    {
        PreviewImage->SetBrushFromTexture(Img);
        FSlateBrush Brush = PreviewImage->GetBrush();
        Brush.DrawAs = ESlateBrushDrawType::Image;
        Brush.ImageSize = FVector2D(1.0f, 1.0f);
        PreviewImage->SetBrush(Brush);
    }
}
