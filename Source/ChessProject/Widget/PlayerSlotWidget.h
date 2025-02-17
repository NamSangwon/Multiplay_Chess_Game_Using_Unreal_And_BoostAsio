// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class CHESSPROJECT_API UPlayerSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

public:
	bool IsEmpty();

	void SetPlayerSlotInfo(const FText info);

	void SetPlayerName(const FText player_name);
	FText GetPlayerName();

public:
	void SetReadyState(const bool bReady) { SetMark(bReady); }
	void SetTurnState(const bool bMyTurn) { SetMark(bMyTurn); }

private:
	void SetMark(const bool bMark);

private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerSlotInfo;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerName;	

	UPROPERTY(meta = (BindWidget))
	class UImage* Background;
};
