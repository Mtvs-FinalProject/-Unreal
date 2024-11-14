// Fill out your copyright notice in the Description page of Project Settings.


//#include "CSR/UI/CreateLevel/DraggableTileItem.h"
//#include "Blueprint/WidgetBlueprintLibrary.h"
//#include "CSR/UI/CreateLevel/TileViewManager.h"
//#include "Templates/SharedPointer.h"

//void UDraggableTileItem::SetOwnerTileView(UTileViewManager* Owner)
//{
//    OwnerTileView = Owner;  // TileViewManager를 설정하여 이후 위치 변경 시 사용
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
//        // FEventReply로 반환되는 DetectDragIfPressed는 실행만 하고, 실제 반환은 FReply로 처리
//        UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
//
//        return FReply::Handled();  // FReply 타입으로 반환
//    }
//    return FReply::Unhandled();
//}
//
//void UDraggableTileItem::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//    if (bIsDragging)  // 드래그 상태인 경우에만 동작
//    {
//        FVector2D CurrentMousePosition = InMouseEvent.GetScreenSpacePosition(); // 현재 마우스 위치
//        FVector2D DragOffset = CurrentMousePosition - InitialMousePosition;      // 드래그 오프셋 계산
//
//        // 드래그된 위치로 아이템 이동
//        SetRenderTranslation(InitialWidgetPosition + DragOffset);  // UI가 마우스를 따라 움직이도록 설정
//
//        // 특정 위치를 넘어서면 위치 교체 로직 호출
//        if (OwnerTileView)
//        {
//            OwnerTileView->CheckAndSwapItems(this, DragOffset.X);  // X축 오프셋을 전달하여 좌측으로 이동했는지 확인
//        }
//    }
//}
//
//void UDraggableTileItem::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//    if (bIsDragging)  // 드래그 중인 경우에만 동작
//    {
//        bIsDragging = false;                       // 드래그 상태 해제
//        //ReleaseMouseCapture();                      // 마우스 캡처 해제
//
//        // 드래그 종료 후 아이템 위치 초기화
//        SetRenderTranslation(FVector2D::ZeroVector);  // UI를 원래 위치로 되돌림
//    }
//}
//
//void UDraggableTileItem::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
//{
//    UDragDropOperation* DragDropOp = NewObject<UDragDropOperation>();
//    DragDropOp->Payload = this;  // 드래그된 위젯 자체를 페이로드로 설정
//    DragDropOp->DefaultDragVisual = this;  // 드래그 시 보이는 비주얼 설정
//    OutOperation = DragDropOp;
//}
