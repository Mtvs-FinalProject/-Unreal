// WBP_CreateLevel_MainInput.cpp

#include "CSR/UI/CreateLevel/CSR_W_CreateLevel_MainInput.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Input/Reply.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Containers/StringConv.h"
#include "Misc/Char.h"
#include "Framework/Application/SlateApplication.h"
#include "GenericPlatform/ITextInputMethodSystem.h"
#include "Application/SlateApplicationBase.h"
#include "GenericPlatform/GenericApplication.h"
#include "Templates/SharedPointer.h"

void UCSR_W_CreateLevel_MainInput::NativeConstruct()
{
    Super::NativeConstruct();

    // Ensure Hint Text is visible initially
    if (CreateLevel_MainInput_HinText)
    {
        CreateLevel_MainInput_HinText->SetVisibility(ESlateVisibility::Visible);
    }

    // Bind the OnTextChanged and OnTextCommitted events to handle focus
    if (CreateLevel_MainInput_EText)
    {
        CreateLevel_MainInput_EText->OnTextChanged.AddDynamic(this, &UCSR_W_CreateLevel_MainInput::OnTextChanged);
        CreateLevel_MainInput_EText->OnTextCommitted.AddDynamic(this, &UCSR_W_CreateLevel_MainInput::OnTextCommitted);
    }
}

FReply UCSR_W_CreateLevel_MainInput::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

    // Optionally set focus to the text box for immediate typing
    //if (CreateLevel_MainInput_HinText)
    //{
    //    CreateLevel_MainInput_HinText->SetVisibility(ESlateVisibility::Hidden);
    //}

    // Hide hint text when text box is clicked
    if (CreateLevel_MainInput_SizeBox || CreateLevel_MainInput_Border || CreateLevel_MainInput_Overlay || CreateLevel_MainInput_EText)
    {
        CreateLevel_MainInput_HinText->SetVisibility(ESlateVisibility::Hidden);
        CreateLevel_MainInput_Border->SetVisibility(ESlateVisibility::Hidden);
        FSlateApplication::Get().SetKeyboardFocus(CreateLevel_MainInput_EText->TakeWidget());
    }

    UE_LOG(LogTemp, Warning, TEXT("NativeOnMouseButtonDown was called!"));

    return FReply::Handled();
}

void UCSR_W_CreateLevel_MainInput::OnTextChanged(const FText& Text)
{
    // 단순히 변경된 문자 수를 로그로 출력하는 용도로 사용
    FString InputString = Text.ToString();
    int32 CharCount = InputString.Len();

    UE_LOG(LogTemp, Warning, TEXT("New CharCount: %d"), CharCount);

    // 힌트 텍스트 숨기기
    if (CreateLevel_MainInput_HinText)
    {
        CreateLevel_MainInput_HinText->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UCSR_W_CreateLevel_MainInput::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    const int32 MaxCharacters = 100;
    FString InputString = Text.ToString();

    if (InputString.Len() > MaxCharacters)
    {
        // 최대 글자 수를 초과하면 잘라서 설정
        FString TrimmedText = InputString.Left(MaxCharacters);
        CreateLevel_MainInput_EText->SetText(FText::FromString(TrimmedText));
    }

    // 힌트 텍스트 처리
    if (CreateLevel_MainInput_HinText)
    {
        if (Text.IsEmpty())
        {
            CreateLevel_MainInput_HinText->SetVisibility(ESlateVisibility::Visible);
        }
    }
    CreateLevel_MainInput_Border->SetVisibility(ESlateVisibility::Visible);
}

FText UCSR_W_CreateLevel_MainInput::GetMainText() const
{
    return (this->CreateLevel_MainInput_EText->GetText());
}
