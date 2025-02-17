// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameWidget.h"

#include <ChessProject/ChessGameInstance.h>

#include <ChessProject/Widget/PlayerSlotWidget.h>

#include "Components/TextBlock.h"

void UInGameWidget::NativeOnInitialized()
{
	WhitePlayerSlot = Cast<UPlayerSlotWidget>(GetWidgetFromName(TEXT("WhitePlayerSlot")));
	if (WhitePlayerSlot)
	{
		WhitePlayerSlot->SetPlayerSlotInfo(FText::FromString(TEXT("White")));
	}

	BlackPlayerSlot = Cast<UPlayerSlotWidget>(GetWidgetFromName(TEXT("BlackPlayerSlot")));
	if (BlackPlayerSlot)
	{
		BlackPlayerSlot->SetPlayerSlotInfo(FText::FromString(TEXT("Black")));
	}

	RoundText = Cast<UTextBlock>(GetWidgetFromName(TEXT("RoundText")));
	CheckStateText = Cast<UTextBlock>(GetWidgetFromName(TEXT("CheckStateText")));
	CountdownText = Cast<UTextBlock>(GetWidgetFromName(TEXT("CountdownText")));
}

void UInGameWidget::SetPlayerToWhiteSlot(const FText player_name)
{
	if (WhitePlayerSlot)
	{
		WhitePlayerSlot->SetPlayerName(player_name);
	}
}
void UInGameWidget::SetPlayerToBlackSlot(const FText player_name)
{
	if (BlackPlayerSlot)
	{
		BlackPlayerSlot->SetPlayerName(player_name);
	}
}

void UInGameWidget::SetTurnMark(const int round)
{
	if (WhitePlayerSlot && BlackPlayerSlot)
	{
		bool bWhiteTurn = (round % 2 == 1);
		bool bBlackTurn = (round % 2 == 0);

		WhitePlayerSlot->SetTurnState(bWhiteTurn);
		BlackPlayerSlot->SetTurnState(bBlackTurn);
	}
}

void UInGameWidget::SetRoundText(const int round)
{
	if (RoundText)
	{
		RoundText->SetText(FText::AsNumber(round));
	}
}

void UInGameWidget::ShowCheckState(const bool bToggle)
{
	if (bToggle)
	{
		CheckStateText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CheckStateText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInGameWidget::ShowCountdown(const bool bToggle)
{
	if (bToggle)
	{
		CountdownText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CountdownText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInGameWidget::UpdateCountdown(const int sec)
{
	CountdownText->SetText(FText::FromString(FString::FromInt(sec)));
}
