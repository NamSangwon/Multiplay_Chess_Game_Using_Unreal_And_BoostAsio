// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class CHESSPROJECT_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	void ClearRoomList();
	void AppendRoom(const int room_index, const std::string room_name, const int room_participants);

private:
	UFUNCTION()
	void OnCreateRoom();

	UFUNCTION()
	void OnProgramEnd();

	UFUNCTION()
	void OnUpdateLobby();

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> RoomSlotWidgetClass;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* RoomListBox;

	UPROPERTY(meta = (BindWidget))
	class UButton* CreateRoomButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ProgramEndButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* UpdateLobbyButton;
};
