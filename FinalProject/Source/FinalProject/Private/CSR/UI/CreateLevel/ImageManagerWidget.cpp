// Fill out your copyright notice in the Description page of Project Settings.


//#include "CSR/UI/CreateLevel/ImageManagerWidget.h"
//#include "IDesktopPlatform.h"
//#include "DesktopPlatformModule.h"
//#include "Engine/Texture2D.h"
//#include "ImageUtils.h"
//#include "Misc/Paths.h"
//#include "Components/ListViewBase.h"


//void UImageManagerWidget::NativeConstruct()
//{
//    Super::NativeConstruct();
//
//    if (ImageTileView)
//    {
//        // TileView 기본 설정
//        FVector2D ViewportSize;
//        if (GEngine && GEngine->GameViewport)
//        {
//            GEngine->GameViewport->GetViewportSize(ViewportSize);
//        }
//
//        const float TileHeight = ViewportSize.Y * 0.3f;
//        const float TileWidth = TileHeight;
//
//        ImageTileView->SetEntryWidth(TileWidth);
//        ImageTileView->SetEntryHeight(TileHeight);
//
//        // 이벤트 바인딩
//        ImageTileView->OnItemClickedInternal.AddDynamic(this, &UImageManagerWidget::OnImageSelectedEvent);
//    }
//
//    if (AddImageButton)
//    {
//        AddImageButton->OnClicked.AddDynamic(this, &UImageManagerWidget::OnAddImageButtonClicked);
//    }
//
//    CurrentSelectedIndex = -1;
//}
//
//void UImageManagerWidget::NativeDestruct()
//{
//    Super::NativeDestruct();
//
//    for (UImageDataObject* ImageData : LoadedImages)
//    {
//        if (ImageData && ImageData->IsValidLowLevel())
//        {
//            ImageData->RemoveFromRoot();
//            ImageData->ConditionalBeginDestroy();
//        }
//    }
//    LoadedImages.Empty();
//}
//
//void UImageManagerWidget::NativeOnSizeChanged(const FGeometry& InGeometry)
//{
//    //Super::NativeOnSizeChanged(InGeometry);
//    UpdateTileViewLayout();
//}
//
//void UImageManagerWidget::UpdateTileViewLayout()
//{
//    if (ImageTileView)
//    {
//        FVector2D ViewportSize;
//        if (GEngine && GEngine->GameViewport)
//        {
//            GEngine->GameViewport->GetViewportSize(ViewportSize);
//        }
//        const float TileViewHeight = ViewportSize.Y * 0.3f;  // TileView 영역의 높이
//        const float TileHeight = TileViewHeight * 0.9f;      // 여백을 위해 90%
//        const float TileWidth = TileHeight;                  // 정사각형 썸네일
//
//        ImageTileView->SetEntryWidth(TileWidth);
//        ImageTileView->SetEntryHeight(TileHeight);
//    }
//}
//
//void UImageManagerWidget::OnAddImageButtonClicked()
//{
//    OpenImagePicker();
//}
//
//void UImageManagerWidget::OpenImagePicker()
//{
//    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
//    if (DesktopPlatform)
//    {
//        TArray<FString> OutFilenames;
//        const FString FileTypes = TEXT("Image Files (*.png;*.jpg;*.jpeg)|*.png;*.jpg;*.jpeg");
//        const FString DefaultPath = LastSelectedFilePath.IsEmpty() ? FPaths::ProjectContentDir() : FPaths::GetPath(LastSelectedFilePath);
//
//        const bool bOpened = DesktopPlatform->OpenFileDialog(
//            nullptr,
//            TEXT("이미지 선택"),
//            DefaultPath,
//            TEXT(""),
//            FileTypes,
//            EFileDialogFlags::Multiple,
//            OutFilenames
//        );
//
//        if (bOpened && OutFilenames.Num() > 0)
//        {
//            LastSelectedFilePath = OutFilenames[0];
//
//            for (const FString& ImagePath : OutFilenames)
//            {
//                UTexture2D* LoadedTexture = FImageUtils::ImportFileAsTexture2D(ImagePath);
//                if (LoadedTexture)
//                {
//                    UImageDataObject* NewImageData = NewObject<UImageDataObject>();
//                    NewImageData->ImageTexture = LoadedTexture;
//                    NewImageData->ImagePath = ImagePath;
//                    NewImageData->Index = LoadedImages.Num();
//                    LoadedImages.Add(NewImageData);
//                }
//            }
//
//            UpdateTileViewItems();
//
//            if (LoadedImages.Num() > 0 && CurrentSelectedIndex == -1)
//            {
//                CurrentSelectedIndex = 0;
//                UpdatePreviewImage(LoadedImages[0]);
//            }
//        }
//    }
//}
//
//void UImageManagerWidget::OnImageSelectedEvent(UObject* SelectedItem)
//{
//    if (UImageDataObject* ImageData = Cast<UImageDataObject>(SelectedItem))
//    {
//        CurrentSelectedIndex = ImageData->Index;
//        UpdatePreviewImage(ImageData);
//    }
//}
//
//void UImageManagerWidget::UpdatePreviewImage(UImageDataObject* ImageData)
//{
//    if (PreviewImage && ImageData && ImageData->ImageTexture)
//    {
//        PreviewImage->SetBrushFromTexture(ImageData->ImageTexture);
//        FSlateBrush Brush = PreviewImage->GetBrush();
//        Brush.DrawAs = ESlateBrushDrawType::Image;
//        Brush.ImageSize = FVector2D(1.0f, 1.0f);
//        PreviewImage->SetBrush(Brush);
//    }
//}
//
//FReply UImageManagerWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
//{
//    if (InKeyEvent.GetKey() == EKeys::Delete && CurrentSelectedIndex >= 0)
//    {
//        DeleteSelectedImage();
//        return FReply::Handled();
//    }
//
//    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
//}
//
//void UImageManagerWidget::DeleteSelectedImage()
//{
//    if (LoadedImages.IsValidIndex(CurrentSelectedIndex))
//    {
//        LoadedImages[CurrentSelectedIndex]->ConditionalBeginDestroy();
//        LoadedImages.RemoveAt(CurrentSelectedIndex);
//
//        // 인덱스 재설정
//        for (int32 i = 0; i < LoadedImages.Num(); ++i)
//        {
//            LoadedImages[i]->Index = i;
//        }
//
//        UpdateTileViewItems();
//
//        if (LoadedImages.Num() > 0)
//        {
//            CurrentSelectedIndex = FMath::Min(CurrentSelectedIndex, LoadedImages.Num() - 1);
//            UpdatePreviewImage(LoadedImages[CurrentSelectedIndex]);
//        }
//        else
//        {
//            CurrentSelectedIndex = -1;
//            if (PreviewImage)
//            {
//                PreviewImage->SetBrushFromTexture(nullptr);
//            }
//        }
//    }
//}
//
//void UImageManagerWidget::UpdateTileViewItems()
//{
//    if (ImageTileView)
//    {
//        ImageTileView->ClearListItems();
//        for (UImageDataObject* ImageData : LoadedImages)
//        {
//            ImageTileView->AddItem(ImageData);
//        }
//    }
//}
//
//void UImageTileEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
//{
//    if (UImageDataObject* ImageData = Cast<UImageDataObject>(ListItemObject))
//    {
//        if (ThumbnailImage)
//        {
//            // 이미지 설정
//            ThumbnailImage->SetBrushFromTexture(ImageData->ImageTexture);
//            FSlateBrush Brush = ThumbnailImage->GetBrush();
//            Brush.DrawAs = ESlateBrushDrawType::Image;
//            Brush.ImageSize = FVector2D(1.0f, 1.0f);
//            ThumbnailImage->SetBrush(Brush);
//        }
//    }
//}
