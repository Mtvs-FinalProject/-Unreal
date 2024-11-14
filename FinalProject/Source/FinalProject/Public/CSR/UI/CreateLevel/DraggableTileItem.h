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
//    // TileView�� �θ� Ŭ���� ����
//    void SetOwnerTileView(class UTileViewManager* Owner);
//
//protected:
//    // �巡�� ���� �̺�Ʈ (���콺 ��ư�� ������ ��)
//    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
//
//    // �巡�� �� (���콺�� ������ �� ȣ��)
//    virtual void NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
//
//    // �巡�� ���� (���콺 ��ư�� ������ �� ȣ��)
//    virtual void NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
//
//    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
//private:
//    // �巡�װ� ���۵� �� �θ� TileView ����
//    UPROPERTY()
//    class UTileViewManager* OwnerTileView;  // ���� TileView�� �����Ͽ� ��ġ ���� ���� ��û�� �� ���
//
//    // �巡�� ���� �� ���콺�� �ʱ� ��ġ
//    FVector2D InitialMousePosition;
//
//    // �巡�� ���� �� ������ �ʱ� ��ġ
//    FVector2D InitialWidgetPosition;
//
//    // �巡�� ���¸� ��Ÿ���� �÷���
//    bool bIsDragging = false;
//};

