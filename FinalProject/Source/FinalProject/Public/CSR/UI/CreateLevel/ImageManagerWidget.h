// Fill out your copyright notice in the Description page of Project Settings.

//#pragma once
//
//#include "CoreMinimal.h"
//#include "Blueprint/UserWidget.h"
//#include "Components/TileView.h"
//#include "Components/Image.h"
//#include "Components/VerticalBox.h"
//#include "Components/Button.h"
//#include "Blueprint/IUserObjectListEntry.h"
//#include "ImageManagerWidget.generated.h"
//
///**
// * 
// */
//
//// Entry Widget Class
//UCLASS()
//class FINALPROJECT_API UImageTileEntryWidget : public UUserWidget, public IUserObjectListEntry
//{
//    GENERATED_BODY()
//
//public:
//    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
//    UImage* ThumbnailImage;
//
//    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
//};
//
//UCLASS(BlueprintType)
//class FINALPROJECT_API UImageDataObject : public UObject
//{
//    GENERATED_BODY()
//
//public:
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Image Data")
//    UTexture2D* ImageTexture;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Image Data")
//    FString ImagePath;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Image Data")
//    int32 Index;
//};

//UCLASS()
//class FINALPROJECT_API UImageManagerWidget : public UUserWidget
//{
//    GENERATED_BODY()


//public:
//    virtual void NativeConstruct() override;
//    virtual void NativeDestruct() override;
//    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
//
//protected:
//    virtual void NativeOnSizeChanged(const FGeometry& InGeometry) override;
//    void UpdateTileViewLayout();
//
//    UPROPERTY(meta = (BindWidget))
//    UImage* PreviewImage;
//    
//    UPROPERTY(meta = (BindWidget))
//    UTileView* ImageTileView;
//    
//    UPROPERTY(meta = (BindWidget))
//    UButton* AddImageButton;
//
//    UFUNCTION()
//    void OnAddImageButtonClicked();
//
//    UFUNCTION()
//    void OnImageSelectedEvent(UObject* SelectedItem);
//
//private:
//    UPROPERTY()
//    TArray<UImageDataObject*> LoadedImages;
//    
//    int32 CurrentSelectedIndex;
//    FString LastSelectedFilePath;
//
//    void UpdatePreviewImage(UImageDataObject* ImageData);
//    void DeleteSelectedImage();
//    void UpdateTileViewItems();
//    void OpenImagePicker();
//};
