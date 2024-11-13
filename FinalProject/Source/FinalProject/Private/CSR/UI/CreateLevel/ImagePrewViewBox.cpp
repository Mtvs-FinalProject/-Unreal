// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/CreateLevel/ImagePrewViewBox.h"
#include "IDesktopPlatform.h"
#include "DesktopPlatformModule.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Engine/Texture2D.h"
#include "ImageUtils.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "Misc/MessageDialog.h"
#include "Styling/SlateBrush.h"

void UImagePrewViewBox::NativeConstruct()
{
    Super::NativeConstruct();

    if (AddImageButton)
    {
        AddImageButton->OnClicked.AddDynamic(this, &UImagePrewViewBox::OnAddImageButtonClicked);
    }
}

void UImagePrewViewBox::OnAddImageButtonClicked()
{
    OpenImageFileDialog();
}

bool UImagePrewViewBox::IsValidPNGFile(const FString& FilePath)
{
    // 파일 확장자 검사
    if (!FilePath.EndsWith(TEXT(".png"), ESearchCase::IgnoreCase))
    {
        FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("PNG 파일만 선택할 수 있습니다.")));
        return false;
    }

    // PNG 시그니처 검사
    TArray<uint8> FileHeader;
    if (FFileHelper::LoadFileToArray(FileHeader, *FilePath))
    {
        const uint8 PNGSignature[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
        if (FileHeader.Num() >= 8)
        {
            return FMemory::Memcmp(FileHeader.GetData(), PNGSignature, 8) == 0;
        }
    }

    FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("유효하지 않은 PNG 파일입니다.")));
    return false;
}

void UImagePrewViewBox::OpenImageFileDialog()
{
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (DesktopPlatform)
    {
        TArray<FString> OutFilenames;
        // PNG 파일만 필터링
        FString ExtensionStr = TEXT("PNG Files (*.png)|*.png");

        const void* ParentWindowHandle = FSlateApplication::Get().GetActiveTopLevelWindow()->GetNativeWindow()->GetOSWindowHandle();

        if (DesktopPlatform->OpenFileDialog(
            ParentWindowHandle,
            TEXT("PNG 이미지 선택"),
            FPaths::GetPath(FPaths::ProjectContentDir()),
            TEXT(""),
            ExtensionStr,
            EFileDialogFlags::None,
            OutFilenames
        ))
        {
            if (OutFilenames.Num() > 0 && IsValidPNGFile(OutFilenames[0]))
            {
                AddImageToPreview(OutFilenames[0]);
            }
        }
    }
}

UTexture2D* UImagePrewViewBox::LoadTextureFromFile(const FString& FilePath)
{
    // 파일 데이터 로드
    TArray<uint8> RawFileData;
    if (!FFileHelper::LoadFileToArray(RawFileData, *FilePath))
    {
        FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("파일을 읽을 수 없습니다.")));
        return nullptr;
    }

    // 이미지 래퍼 모듈 로드
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

    if (!ImageWrapper.IsValid())
    {
        FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("이미지 래퍼를 생성할 수 없습니다.")));
        return nullptr;
    }

    // 이미지 압축 데이터 설정
    if (!ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
    {
        FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("이미지 데이터가 유효하지 않습니다.")));
        return nullptr;
    }

    // BGRA 포맷으로 압축 해제
    TArray<uint8> UncompressedBGRA;
    if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
    {
        FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("이미지 압축을 해제할 수 없습니다.")));
        return nullptr;
    }

    // 텍스처 생성
    UTexture2D* NewTexture = UTexture2D::CreateTransient(
        ImageWrapper->GetWidth(),
        ImageWrapper->GetHeight(),
        PF_B8G8R8A8
    );

    if (!NewTexture)
    {
        FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("텍스처를 생성할 수 없습니다.")));
        return nullptr;
    }

    // 텍스처 데이터 설정
    FTexturePlatformData* PlatformData = NewTexture->GetPlatformData();
    if (PlatformData)
    {
        void* TextureData = PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
        FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
        PlatformData->Mips[0].BulkData.Unlock();

        // 텍스처 설정
        NewTexture->AddToRoot();  // GC 방지
        NewTexture->UpdateResource();

        // 이미지 경로 저장
        LoadedImagePaths.Add(FilePath);

        return NewTexture;
    }

    FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("텍스처 데이터를 설정할 수 없습니다.")));
    return nullptr;
}

void UImagePrewViewBox::AddImageToPreview(const FString& ImagePath)
{
    UTexture2D* NewTexture = LoadTextureFromFile(ImagePath);
    if (NewTexture)
    {
        // 썸네일 이미지 생성
        UImage* ThumbnailImage = NewObject<UImage>(this);
        ThumbnailImage->SetBrushFromTexture(NewTexture);

        // 썸네일 크기 설정 - 스크롤박스의 높이에 맞춤
        float ScrollBoxHeight = ThumbnailScrollBox->GetCachedGeometry().GetLocalSize().Y;
        ThumbnailImage->SetDesiredSizeOverride(FVector2D(ScrollBoxHeight * 1.5f, ScrollBoxHeight));

        // 썸네일 버튼 생성
        UButton* ThumbnailButton = NewObject<UButton>(this);
        ThumbnailButton->AddChild(ThumbnailImage);

        // 클릭 이벤트 바인딩
        FScriptDelegate ClickDelegate;
        ClickDelegate.BindUFunction(this, "OnThumbnailClicked");
        ThumbnailButton->OnClicked.Add(ClickDelegate);

        // 버튼 배열에 추가
        ThumbnailButtons.Add(ThumbnailButton);

        // 스크롤 박스에 썸네일 추가
        ThumbnailScrollBox->AddChild(ThumbnailButton);

        // 첫 이미지인 경우 메인 이미지로 설정
        if (LoadedImagePaths.Num() == 0)
        {
            UpdateMainImage(NewTexture);
        }

        LoadedImagePaths.Add(ImagePath);
    }
}

void UImagePrewViewBox::OnThumbnailClicked(UButton* ClickedButton)
{
    if (ClickedButton)
    {
        UImage* ClickedImage = Cast<UImage>(ClickedButton->GetChildAt(0));
        if (ClickedImage)
        {
            UTexture2D* ClickedTexture = Cast<UTexture2D>(ClickedImage->Brush.GetResourceObject());
            if (ClickedTexture)
            {
                UpdateMainImage(ClickedTexture);
            }
        }
    }
}

void UImagePrewViewBox::UpdateMainImage(UTexture2D* NewTexture)
{
    if (MainImageWidget && NewTexture)
    {
        MainImageWidget->SetBrushFromTexture(NewTexture);
    }
}