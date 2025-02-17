// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class CHESSPROJECT_API UInGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

public:
	void SetPlayerToWhiteSlot(const FText player_name);
	void SetPlayerToBlackSlot(const FText player_name);

public:
	void SetTurnMark(const int round);
	void SetRoundText(const int round);

public:
	void ShowCheckState(const bool bToggle);
	void ShowCountdown(const bool bToggle);
	void UpdateCountdown(const int sec);

private:
	UPROPERTY(meta = (BindWidget))
	class UPlayerSlotWidget* WhitePlayerSlot;

	UPROPERTY(meta = (BindWidget))
	class UPlayerSlotWidget* BlackPlayerSlot;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RoundText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CheckStateText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CountdownText;
};
