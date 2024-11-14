// Fill out your copyright notice in the Description page of Project Settings.


#include "CSR/UI/CreateLevel/TitleBox.h"
#include "Styling/SlateTypes.h"

FText UTitleBox::GetTitle() const
{
	return (Title->GetText());
}
