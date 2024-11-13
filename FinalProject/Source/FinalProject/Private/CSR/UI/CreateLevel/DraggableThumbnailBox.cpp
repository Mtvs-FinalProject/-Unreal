// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/CreateLevel/DraggableThumbnailBox.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"

UDraggableThumbnailBox::UDraggableThumbnailBox(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , bIsDragging(false)
    , DraggedButton(nullptr)
    , DraggedIndex(-1)
    , DragOffset(FVector2D::ZeroVector)
{
}

void UDraggableThumbnailBox::NativeConstruct()
{
    Super::NativeConstruct();

    if (ContentScrollBox)
    {
        // ��ũ�ѹڽ��� �������� ����
        ContentScrollBox->SetOrientation(Orient_Horizontal);

        // ��ũ�ѹ� �����
        ContentScrollBox->SetScrollBarVisibility(ESlateVisibility::Hidden);
    }
}

void UDraggableThumbnailBox::AddChild(UWidget* Content)
{
    if (ContentScrollBox && Content)
    {
        // ��ũ�ѹڽ��� ���� ���� ��������
        float BoxHeight = ContentScrollBox->GetCachedGeometry().GetLocalSize().Y;

        // �̹��� ũ�� ����
        if (UImage* ImageWidget = Cast<UImage>(Content))
        {
            UTexture2D* Texture = Cast<UTexture2D>(ImageWidget->Brush.GetResourceObject());
            if (Texture)
            {
                // �̹����� ������ ���� ũ�� ���
                float AspectRatio = static_cast<float>(Texture->GetSizeX()) / Texture->GetSizeY();
                float BoxHeight = ContentScrollBox->GetCachedGeometry().GetLocalSize().Y;
                float NewWidth = BoxHeight * AspectRatio;

                // �̹��� ������ �귯�� ����
                ImageWidget->SetBrushFromTexture(Texture);
                ImageWidget->Brush.ImageSize = FVector2D(NewWidth, BoxHeight);

                // Stretch ����
                ImageWidget->Brush.SetDrawAs(ESlateBrushDrawType::Image);
                ImageWidget->Brush.Stretch = EStretch::ScaleToFit;  // �Ǵ� ScaleToFill
            }
            ContentScrollBox->AddChild(Content);
        }

        //ContentScrollBox->AddChild(Content);
    }
}

bool UDraggableThumbnailBox::RemoveChild(UWidget* Content)
{
    if (ContentScrollBox && Content)
    {
        return ContentScrollBox->RemoveChild(Content);
    }
    return false;
}

FReply UDraggableThumbnailBox::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && ContentScrollBox)
    {
        FVector2D LocalMousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

        for (int32 i = 0; i < ContentScrollBox->GetChildrenCount(); i++)
        {
            UButton* Button = Cast<UButton>(ContentScrollBox->GetChildAt(i));
            if (Button)
            {
                FGeometry ButtonGeometry = Button->GetCachedGeometry();
                if (ButtonGeometry.IsUnderLocation(InMouseEvent.GetScreenSpacePosition()))
                {
                    bIsDragging = true;
                    DraggedButton = Button;
                    DraggedIndex = i;
                    DragOffset = LocalMousePos;
                    return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
                }
            }
        }
    }

    return FReply::Unhandled();
}

void UDraggableThumbnailBox::ReorderChildren(int32 OldIndex, int32 NewIndex)
{
    if (!ContentScrollBox || OldIndex == NewIndex || !IsValidChildIndex(OldIndex) || !IsValidChildIndex(NewIndex))
        return;

    TArray<UWidget*> Children;
    for (int32 i = 0; i < ContentScrollBox->GetChildrenCount(); ++i)
    {
        Children.Add(ContentScrollBox->GetChildAt(i));
    }

    UWidget* MovedWidget = Children[OldIndex];
    Children.RemoveAt(OldIndex);
    Children.Insert(MovedWidget, NewIndex);

    ContentScrollBox->ClearChildren();
    for (UWidget* Child : Children)
    {
        ContentScrollBox->AddChild(Child);
    }

    OnThumbnailReorderedEvents.Broadcast(OldIndex, NewIndex);
}

bool UDraggableThumbnailBox::IsValidChildIndex(int32 Index) const
{
    return ContentScrollBox && Index >= 0 && Index < ContentScrollBox->GetChildrenCount();
}

FReply UDraggableThumbnailBox::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (bIsDragging)
    {
        bIsDragging = false;
        DraggedButton = nullptr;
        DraggedIndex = -1;
    }

    return FReply::Handled();
}

FReply UDraggableThumbnailBox::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (bIsDragging && DraggedButton && ContentScrollBox)
    {
        FVector2D LocalMousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

        int32 NewIndex = DraggedIndex;
        for (int32 i = 0; i < ContentScrollBox->GetChildrenCount(); i++)
        {
            if (i != DraggedIndex)
            {
                UWidget* Child = ContentScrollBox->GetChildAt(i);
                if (Child)
                {
                    FGeometry ChildGeometry = Child->GetCachedGeometry();
                    float ChildCenter = ChildGeometry.GetAbsolutePosition().X + ChildGeometry.GetAbsoluteSize().X / 2;
                    if (InMouseEvent.GetScreenSpacePosition().X < ChildCenter)
                    {
                        NewIndex = i;
                        break;
                    }
                }
            }
        }

        if (NewIndex != DraggedIndex)
        {
            ReorderChildren(DraggedIndex, NewIndex);
            DraggedIndex = NewIndex;
        }
    }

    return FReply::Handled();
}