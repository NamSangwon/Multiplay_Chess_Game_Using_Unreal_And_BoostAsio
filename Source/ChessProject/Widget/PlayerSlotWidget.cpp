// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSlotWidget.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"

void UPlayerSlotWidget::NativeOnInitialized()
{
	PlayerSlotInfo = Cast<UTextBlock>(GetWidgetFromName(TEXT("PlayerSlotInfo")));
	PlayerName = Cast<UTextBlock>(GetWidgetFromName(TEXT("PlayerName")));
	Background = Cast<UImage>(GetWidgetFromName(TEXT("Background")));
}

bool UPlayerSlotWidget::IsEmpty()
{
	if (PlayerName)
	{
		return (PlayerName->GetText().IsEmpty());
	}

	return false;
}

void UPlayerSlotWidget::SetPlayerSlotInfo(const FText info)
{
	if (PlayerSlotInfo)
	{
		PlayerSlotInfo->SetText(info);
	}
}

void UPlayerSlotWidget::SetPlayerName(const FText player_name)
{
	if (PlayerName)
	{
		PlayerName->SetText(player_name);
	}
}

FText UPlayerSlotWidget::GetPlayerName()
{
	if (PlayerName)
	{
		return PlayerName->GetText();
	}

	return FText::GetEmpty();
}

void UPlayerSlotWidget::SetMark(const bool bMark)
{
	if (Background)
	{
		if (bMark)
		{
			Background->SetColorAndOpacity(FLinearColor::Red);
		}
		else
		{
			Background->SetColorAndOpacity(FLinearColor::Black);
		}
	}
}
