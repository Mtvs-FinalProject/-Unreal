// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CSR_W_CreateLevel_MainInput.generated.h"

/**
 * 
 */

UCLASS()
class FINALPROJECT_API UCSR_W_CreateLevel_MainInput : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UFUNCTION()
    FText GetMainText() const;

protected:
    // Bind the Editable Text Box and Hint Text in C++
    UPROPERTY(meta = (BindWidget))
    class UMultiLineEditableTextBox* CreateLevel_MainInput_EText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* CreateLevel_MainInput_HinText;

    UPROPERTY(meta = (BindWidget))
    class UOverlay* CreateLevel_MainInput_Overlay;

    UPROPERTY(meta = (BindWidget))
    class UBorder* CreateLevel_MainInput_Border;

    UPROPERTY(meta = (BindWidget))
    class USizeBox* CreateLevel_MainInput_SizeBox;

    // Override OnMouseButtonDown to hide hint text on click
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    UFUNCTION()
    void OnTextChanged(const FText& Text);

    UFUNCTION()
    void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
};
