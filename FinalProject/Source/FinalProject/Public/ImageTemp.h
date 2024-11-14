// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "IDesktopPlatform.h"
#include "DesktopPlatformModule.h"
#include "Engine/Texture2D.h"
#include "ImageTemp.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJECT_API UImageTemp : public UUserWidget
{
	GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    UImage* PreviewImage;

    UPROPERTY(meta = (BindWidget))
    UButton* AddImageButton;

    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnAddImageButtonClicked();

    UFUNCTION()
    TArray<FString> GetImageList() const;
private:
    FString LastSelectedFilePath;

    void OpenImagePicker();

    void UpdatePreviewImage(class UTexture2D *Img);
};
