// Fill out your copyright notice in the Description page of Project Settings.

//#pragma once
//
//#include "CoreMinimal.h"
//#include "Blueprint/UserWidget.h"
//#include "Components/ScrollBox.h"
//#include "Components/Button.h"
//#include "Framework/Application/IInputProcessor.h"
//#include "Framework/Application/SlateApplication.h"
//#include "Input/Events.h"
//#include "Input/Reply.h"
//#include "Layout/Geometry.h"
//#include "Widgets/Input/SButton.h"
//#include "Delegates/DelegateCombinations.h"
//#include "DraggableThumbnailBox.generated.h"

/**
 * 
 */

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnThumbnailReorderedEvents, int32, OldIndex, int32, NewIndex);

//UCLASS()
//class FINALPROJECT_API UDraggableThumbnailBox : public UUserWidget
//{
//	GENERATED_BODY()
//
//public:
//    UDraggableThumbnailBox(const FObjectInitializer& ObjectInitializer);
//
//	UPROPERTY(BlueprintAssignable, Category = "Events")
//	FOnThumbnailReorderedEvents OnThumbnailReorderedEvents;
//
//    // ScrollBox에 자식 위젯 추가
//    UFUNCTION(BlueprintCallable, Category = "Widget")
//    void AddChild(UWidget* Content);
//
//    // ScrollBox 자식 위젯 제거
//    UFUNCTION(BlueprintCallable, Category = "Widget")
//    bool RemoveChild(UWidget* Content);
//
//protected:
//    virtual void NativeConstruct() override;
//    virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
//    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
//    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
//
//    UPROPERTY(meta = (BindWidget))
//    UScrollBox* ContentScrollBox;
//
//private:
//    UPROPERTY()
//    bool bIsDragging;
//
//    UPROPERTY()
//    UButton* DraggedButton;
//
//    UPROPERTY()
//    int32 DraggedIndex;
//
//    UPROPERTY()
//    FVector2D DragOffset;
//
//    void ReorderChildren(int32 OldIndex, int32 NewIndex);
//    bool IsValidChildIndex(int32 Index) const;
//};
