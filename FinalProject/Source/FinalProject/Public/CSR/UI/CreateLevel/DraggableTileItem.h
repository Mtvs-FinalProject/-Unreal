// Fill out your copyright notice in the Description page of Project Settings.

//#pragma once
//
//#include "CoreMinimal.h"
//#include "Blueprint/UserWidget.h"
////#include "DraggableTileItem.generated.h"
//
///**
// * 
// */
//UCLASS()
//class FINALPROJECT_API UDraggableTileItem : public UUserWidget, public TSharedFromThis<UDraggableTileItem>
//{
//	GENERATED_BODY()

//public:
//    // TileView의 부모 클래스 설정
//    void SetOwnerTileView(class UTileViewManager* Owner);
//
//protected:
//    // 드래그 시작 이벤트 (마우스 버튼이 눌렸을 때)
//    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
//
//    // 드래그 중 (마우스가 움직일 때 호출)
//    virtual void NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
//
//    // 드래그 종료 (마우스 버튼을 놓았을 때 호출)
//    virtual void NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
//
//    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
//private:
//    // 드래그가 시작될 때 부모 TileView 설정
//    UPROPERTY()
//    class UTileViewManager* OwnerTileView;  // 소유 TileView를 참조하여 위치 변경 등을 요청할 때 사용
//
//    // 드래그 시작 시 마우스의 초기 위치
//    FVector2D InitialMousePosition;
//
//    // 드래그 시작 시 위젯의 초기 위치
//    FVector2D InitialWidgetPosition;
//
//    // 드래그 상태를 나타내는 플래그
//    bool bIsDragging = false;
//};

