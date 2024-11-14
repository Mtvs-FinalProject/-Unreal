// Fill out your copyright notice in the Description page of Project Settings.


//#include "CSR/UI/CreateLevel/DraggableTileItem.h"
//#include "Blueprint/WidgetBlueprintLibrary.h"
//#include "CSR/UI/CreateLevel/TileViewManager.h"
//#include "Templates/SharedPointer.h"

//void UDraggableTileItem::SetOwnerTileView(UTileViewManager* Owner)
//{
//    OwnerTileView = Owner;  // TileViewManager�� �����Ͽ� ���� ��ġ ���� �� ���
//}
//
//FReply UDraggableTileItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//    if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
//    {
//        InitialMousePosition = InMouseEvent.GetScreenSpacePosition();
//        InitialWidgetPosition = RenderTransform.Translation;
//        bIsDragging = true;
//
//        // FEventReply�� ��ȯ�Ǵ� DetectDragIfPressed�� ���ุ �ϰ�, ���� ��ȯ�� FReply�� ó��
//        UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
//
//        return FReply::Handled();  // FReply Ÿ������ ��ȯ
//    }
//    return FReply::Unhandled();
//}
//
//void UDraggableTileItem::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//    if (bIsDragging)  // �巡�� ������ ��쿡�� ����
//    {
//        FVector2D CurrentMousePosition = InMouseEvent.GetScreenSpacePosition(); // ���� ���콺 ��ġ
//        FVector2D DragOffset = CurrentMousePosition - InitialMousePosition;      // �巡�� ������ ���
//
//        // �巡�׵� ��ġ�� ������ �̵�
//        SetRenderTranslation(InitialWidgetPosition + DragOffset);  // UI�� ���콺�� ���� �����̵��� ����
//
//        // Ư�� ��ġ�� �Ѿ�� ��ġ ��ü ���� ȣ��
//        if (OwnerTileView)
//        {
//            OwnerTileView->CheckAndSwapItems(this, DragOffset.X);  // X�� �������� �����Ͽ� �������� �̵��ߴ��� Ȯ��
//        }
//    }
//}
//
//void UDraggableTileItem::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//    if (bIsDragging)  // �巡�� ���� ��쿡�� ����
//    {
//        bIsDragging = false;                       // �巡�� ���� ����
//        //ReleaseMouseCapture();                      // ���콺 ĸó ����
//
//        // �巡�� ���� �� ������ ��ġ �ʱ�ȭ
//        SetRenderTranslation(FVector2D::ZeroVector);  // UI�� ���� ��ġ�� �ǵ���
//    }
//}
//
//void UDraggableTileItem::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
//{
//    UDragDropOperation* DragDropOp = NewObject<UDragDropOperation>();
//    DragDropOp->Payload = this;  // �巡�׵� ���� ��ü�� ���̷ε�� ����
//    DragDropOp->DefaultDragVisual = this;  // �巡�� �� ���̴� ���־� ����
//    OutOperation = DragDropOp;
//}
