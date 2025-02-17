// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoomWidget.generated.h"

/**
 * 
 */
UCLASS()
class CHESSPROJECT_API URoomWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

public:
	void SetRoomName(const FText room_name);
	void SetMySlot(const FText PlayerName, const bool bReady);
	void SetOppositeSlot(const FText PlayerName, const bool bReady);

private:
	UFUNCTION()
	void OnReadyToPlayGame();

	UFUNCTION()
	void OnLeaveRoom();

private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RoomName;

	UPROPERTY(meta = (BindWidget))
	class UButton* ReadyButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* LeaveButton;

	UPROPERTY(meta = (BindWidget))
	class UPlayerSlotWidget* MySlot;

	UPROPERTY(meta = (BindWidget))
	class UPlayerSlotWidget* OppositeSlot;

private:
	bool m_bReadyToPlay = false;
};
