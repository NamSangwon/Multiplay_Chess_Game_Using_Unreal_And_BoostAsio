// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoomSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class CHESSPROJECT_API URoomSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	void SetIndex(const int room_index);
	void SetName(const FText room_name);
	void SetParticipants(const int room_participants);

private:
	UFUNCTION()
	void OnEnterRoom();

private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RoomIndex;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RoomName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RoomParticipants;

	UPROPERTY(meta = (BindWidget))
	class UButton* RoomButton;
};
