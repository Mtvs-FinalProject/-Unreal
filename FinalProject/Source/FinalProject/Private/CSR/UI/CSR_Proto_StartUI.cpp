// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/CSR_Proto_StartUI.h"

void UCSR_Proto_StartUI::NativeConstruct()
{
	if (this->BTN_CreateRoom)
    {
        this->BTN_CreateRoom->OnClicked.AddDynamic(this, &UCSR_Proto_StartUI::OnCreateRoomClicked);
    }

    if (this->BTN_LoadRoom)
    {
        this->BTN_LoadRoom->OnClicked.AddDynamic(this, &UCSR_Proto_StartUI::OnLoadRoomClicked);
    }
	if (this->BTN_Exit)
    {
        this->BTN_Exit->OnClicked.AddDynamic(this, &UCSR_Proto_StartUI::OnExitClicked);
    }
}

