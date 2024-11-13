// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/CreateLevel/ImagePrewViewBox.h"
#include "IDesktopPlatform.h"
#include "DesktopPlatformModule.h"
#include "HAL/PlatformApplicationMisc.h"
#include "ImageUtils.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "Misc/MessageDialog.h"

void UImagePrewViewBox::NativeConstruct()
{
    Super::NativeConstruct();

    if (AddImageButton)
    {
		AddImageButton->OnClicked.AddDynamic(this, &UImagePrewViewBox::OnAddImageButtonClicked);
    }

    if (ThumbnailScrollBox)
    {
        ThumbnailScrollBox->OnThumbnailReorderedEvents.AddDynamic(this, &UImagePrewViewBox::OnThumbnailOrderChanged);
    }
}

void UImagePrewViewBox::OnAddImageButtonClicked()
{
    OpenImageFileDialog();
}

void UImagePrewViewBox::OpenImageFileDialog()
{
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (!DesktopPlatform) return;

    TArray<FString> OutFilenames;
    const FString ExtensionStr = TEXT("PNG Files (*.png)|*.png");
    const void* ParentWindowHandle = FSlateApplication::Get().GetActiveTopLevelWindow()->GetNativeWindow()->GetOSWindowHandle();

    if (DesktopPlatform->OpenFileDialog(
        ParentWindowHandle,
        TEXT("PNG 이미지 선택"),
        FPaths::GetPath(FPaths::ProjectContentDir()),
        TEXT(""),
        ExtensionStr,
        EFileDialogFlags::None,
        OutFilenames))
    {
        if (OutFilenames.Num() > 0 && IsValidPNGFile(OutFilenames[0]))
        {
            AddImageToPreview(OutFilenames[0]);
        }
    }
}

bool UImagePrewViewBox::IsValidPNGFile(const FString& FilePath)
{
    if (!FilePath.EndsWith(TEXT(".png"), ESearchCase::IgnoreCase))
    {
        FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("PNG 파일만 선택할 수 있습니다.")));
        return false;
    }

    TArray<uint8> FileHeader;
    if (FFileHelper::LoadFileToArray(FileHeader, *FilePath))
    {
        const uint8 PNGSignature[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
        if (FileHeader.Num() >= 8)
        {
            if (FMemory::Memcmp(FileHeader.GetData(), PNGSignature, 8) == 0)
            {
                return true;
            }
        }
    }

    FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("유효하지 않은 PNG 파일입니다.")));
    return false;
}

UTexture2D* UImagePrewViewBox::LoadTextureFromFile(const FString& FilePath)
{
    TArray<uint8> RawFileData;
    if (!FFileHelper::LoadFileToArray(RawFileData, *FilePath))
    {
        return nullptr;
    }

    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

    if (!ImageWrapper.IsValid() || !ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
    {
        return nullptr;
    }

    TArray<uint8> UncompressedBGRA;
    if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
    {
        return nullptr;
    }

    UTexture2D* NewTexture = UTexture2D::CreateTransient(
        ImageWrapper->GetWidth(),
        ImageWrapper->GetHeight(),
        PF_B8G8R8A8
    );

    if (!NewTexture)
    {
        return nullptr;
    }

    void* TextureData = NewTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
    FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
    NewTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
    NewTexture->UpdateResource();

    return NewTexture;
}

void UImagePrewViewBox::AddImageToPreview(const FString& ImagePath)
{
    UTexture2D* NewTexture = LoadTextureFromFile(ImagePath);
    if (!NewTexture) return;

    // 썸네일 데이터 생성
    UThumbnailData* ThumbnailData = NewObject<UThumbnailData>(this);
    ThumbnailData->Initialize(NewTexture, ImagePath);
    ThumbnailData->ThumbnailIndex = ThumbnailDataArray.Num();

    // 썸네일 버튼 생성
    float ScrollBoxHeight = ThumbnailScrollBox->GetCachedGeometry().GetLocalSize().Y;
    UButton* ThumbnailButton = ThumbnailData->CreateThumbnailButton(this, ScrollBoxHeight * 0.8f);

    if (!ThumbnailButton) return;

    // 버튼 클릭 이벤트 바인딩
    FScriptDelegate ButtonDelegate;
    ButtonDelegate.BindUFunction(this, "OnThumbnailButtonClicked");
    ThumbnailButton->OnClicked.Add(ButtonDelegate);

    // 데이터 저장
    ThumbnailDataArray.Add(ThumbnailData);
    CurrentSelectedData = ThumbnailData;
    CurrentSelectedButton = ThumbnailButton;

    // 스크롤박스에 추가
    ThumbnailScrollBox->AddChild(ThumbnailButton);

    // 메인 이미지 업데이트
    UpdateMainImage(NewTexture);
}

void UImagePrewViewBox::OnThumbnailButtonClicked()
{
    if (UButton* ClickedButton = Cast<UButton>(CurrentSelectedButton))
    {
        if (UThumbnailData* ThumbnailData = FindThumbnailDataByButton(ClickedButton))
        {
            CurrentSelectedData = ThumbnailData;
            UpdateMainImage(ThumbnailData->Texture);
        }
    }
}

void UImagePrewViewBox::OnThumbnailOrderChanged(int32 OldIndex, int32 NewIndex)
{
    if (ThumbnailDataArray.IsValidIndex(OldIndex) && ThumbnailDataArray.IsValidIndex(NewIndex))
    {
        // 데이터 배열 순서 변경
        ThumbnailDataArray.Swap(OldIndex, NewIndex);

        // 인덱스 업데이트
        for (int32 i = 0; i < ThumbnailDataArray.Num(); ++i)
        {
            if (ThumbnailDataArray[i])
            {
                ThumbnailDataArray[i]->ThumbnailIndex = i;
            }
        }
    }
}

void UImagePrewViewBox::UpdateMainImage(UTexture2D* NewTexture)
{
    if (MainImageWidget && NewTexture && IsValid(MainImageWidget))
    {
        MainImageWidget->SetBrushFromTexture(NewTexture);
    }
}

UThumbnailData* UImagePrewViewBox::FindThumbnailDataByButton(const UButton* Button) const
{
    if (!Button) return nullptr;

    for (UThumbnailData* Data : ThumbnailDataArray)
    {
        if (Data && Data->ThumbnailButton == Button)
        {
            return Data;
        }
    }

    return nullptr;
}