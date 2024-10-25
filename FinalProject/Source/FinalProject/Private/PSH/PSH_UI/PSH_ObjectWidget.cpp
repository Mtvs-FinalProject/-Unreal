// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/PSH_UI/PSH_ObjectWidget.h"
#include "PSH/PSH_Actor/PSH_BlockActor.h"
#include "PSH/PSH_DataTable/PSH_ObjectDataTable.h"
#include "Engine/DataTable.h"
#include "PSH/PSH_UI/PSH_BlockWidget.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "PSH/PSH_Player/PSH_Player.h"
#include "PSH/PSH_Player/PSH_PlayerController.h"
#include "PSH/PSH_Actor/PSH_GarbageBot.h"
#include "Kismet/GameplayStatics.h"

void UPSH_ObjectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btr_Back->OnClicked.AddDynamic(this, &UPSH_ObjectWidget::OnClickedBack);
	Btr_Save->OnClicked.AddDynamic(this, &UPSH_ObjectWidget::OnClickedSave);
	Btr_Load->OnClicked.AddDynamic(this, &UPSH_ObjectWidget::OnClickedLoad);
	Btr_CallBot->OnClicked.AddDynamic(this, &UPSH_ObjectWidget::OnClickedCallBot);

	AddchildBlock();
}
void UPSH_ObjectWidget::AddchildBlock()
{
	TArray<FPSH_UiBlockData*> dataAraay;
	ObjectData->GetAllRows<FPSH_UiBlockData>(TEXT("non"), dataAraay);
	
	for (int i = 0; i < dataAraay.Num(); i++)
	{
		auto* blockWidget = CreateWidget<UPSH_BlockWidget>(GetWorld(), blockWidgetFac);
		FName Rowname = FName(*FString::FormatAsNumber(i));

		FButtonStyle btrStyle;
		FSlateBrush normal;
		FSlateBrush hover;
		FSlateBrush pressed;

		normal.SetResourceObject(dataAraay[i]->icon);
		btrStyle.SetNormal(normal);

		hover.SetResourceObject(dataAraay[i]->icon);
		btrStyle.SetHovered(hover);

		pressed.SetResourceObject(dataAraay[i]->icon);
		btrStyle.SetPressed(pressed);

		blockWidget->Btn_Icon->SetStyle(btrStyle);
		
		blockWidget->spawnActor = dataAraay[i]->actor;
		blockWidget->SetPadding(50.0f);
		Scroll_NomarlBlcok->AddChild(blockWidget);
		
	}

// 	// 그 안에 msg를 반영하고
// 	chetwidget->Text_msg->SetText(FText::FromString(msg));
// 	// 생성된 위젯을 스크롤에 추가하고싶다.
// 	Scroll_MasgList->AddChild(chetwidget);
// 	Scroll_MasgList->ScrollToEnd(); // 제일 아래에 있는 목록을 보도록 스크롤 조정.
}
void UPSH_ObjectWidget::OnClickedBack()
{
	SetVisibility(ESlateVisibility::Hidden);
}
void UPSH_ObjectWidget::OnClickedSave()
{
	APSH_Player* player = Cast<APSH_Player>(GetWorld()->GetFirstPlayerController()->GetCharacter());

	if (player == nullptr || player->pc == nullptr) return;

	player->pc->ObjectSave();
	
}
void UPSH_ObjectWidget::OnClickedLoad()
{
	APSH_Player* player = Cast<APSH_Player>(GetWorld()->GetFirstPlayerController()->GetCharacter());

	if (player == nullptr) return;

	player->pc->ObjectLoad();

}
void UPSH_ObjectWidget::OnClickedCallBot()
{
	APSH_GarbageBot * bot = Cast<APSH_GarbageBot>(UGameplayStatics::GetActorOfClass(GetWorld(),APSH_GarbageBot::StaticClass()));
	APSH_Player* player = Cast<APSH_Player>(GetWorld()->GetFirstPlayerController()->GetCharacter());

	if (bot && player)
	{
		FVector playerLoc = player->GetActorLocation();
		// 곱하기 10 해놓고 갈게유!
		bot->SetActorLocation(player->GetActorLocation() + (player->GetActorForwardVector() * 500) );
		bot->InitializeMovePoint();
	}
}