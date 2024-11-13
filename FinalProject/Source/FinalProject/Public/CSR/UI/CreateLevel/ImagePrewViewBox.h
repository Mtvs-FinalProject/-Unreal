// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "ImagePrewViewBox.generated.h"

/**
 * 
 */

UCLASS()
class FINALPROJECT_API UImagePrewViewBox : public UUserWidget
{
	GENERATED_BODY()


protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UImage* MainImageWidget;

    UPROPERTY(meta = (BindWidget))
    class UScrollBox* ThumbnailScrollBox;

    UPROPERTY(meta = (BindWidget))
    class UButton* AddImageButton;

    UFUNCTION()
    void OnAddImageButtonClicked();

    UFUNCTION()
    void OnThumbnailClicked(UButton* ClickedButton);

private:
    void OpenImageFileDialog();
    void AddImageToPreview(const FString& ImagePath);
    void UpdateMainImage(UTexture2D* NewTexture);
    bool IsValidPNGFile(const FString& FilePath);
    
    UPROPERTY()
    TArray<FString> LoadedImagePaths;
    
    UPROPERTY()
    TArray<UButton*> ThumbnailButtons;
    
    UTexture2D* LoadTextureFromFile(const FString& FilePath);
};
