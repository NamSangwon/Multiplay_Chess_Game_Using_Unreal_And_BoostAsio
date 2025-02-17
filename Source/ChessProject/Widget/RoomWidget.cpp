// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include <ChessProject/Network/ChessGamePacket.hpp>
#include <ChessProject/Network/MyNetworkSubsystem.h>
#include <ChessProject/Widget/PlayerSlotWidget.h>

void URoomWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RoomName = Cast<UTextBlock>(GetWidgetFromName(TEXT("RoomName")));
	MySlot = Cast<UPlayerSlotWidget>(GetWidgetFromName(TEXT("MySlot")));
	if (MySlot)
	{
		MySlot->SetPlayerSlotInfo(FText::FromString(TEXT("My")));
		MySlot->SetReadyState(false);
	}

	OppositeSlot = Cast<UPlayerSlotWidget>(GetWidgetFromName(TEXT("OppositeSlot")));
	if (OppositeSlot)
	{
		OppositeSlot->SetPlayerSlotInfo(FText::FromString(TEXT("Opposite")));
		OppositeSlot->SetReadyState(false);
	}

	ReadyButton = Cast<UButton>(GetWidgetFromName(TEXT("ReadyButton")));
	if (ReadyButton)
	{
		ReadyButton->OnClicked.AddUniqueDynamic(this, &URoomWidget::OnReadyToPlayGame);
	}

	LeaveButton = Cast<UButton>(GetWidgetFromName(TEXT("LeaveButton")));
	if (LeaveButton)
	{
		LeaveButton->OnClicked.AddUniqueDynamic(this, &URoomWidget::OnLeaveRoom);
	}
}

void URoomWidget::SetRoomName(const FText room_name)
{
	if (RoomName)
	{
		RoomName->SetText(room_name);
	}
}

void URoomWidget::SetMySlot(const FText PlayerName, const bool bReady)
{
	if (MySlot)
	{
		m_bReadyToPlay = bReady;
		MySlot->SetPlayerName(PlayerName);
		MySlot->SetReadyState(bReady);
	}
}

void URoomWidget::SetOppositeSlot(const FText PlayerName, const bool bReady)
{
	if (OppositeSlot)
	{
		OppositeSlot->SetPlayerName(PlayerName);

		OppositeSlot->SetReadyState(bReady);
	}
}

void URoomWidget::OnReadyToPlayGame()
{
	if (!OppositeSlot->IsEmpty())
	{
		m_bReadyToPlay = !m_bReadyToPlay;

		UMyNetworkSubsystem* MyNetworkSystem = GetGameInstance()->GetSubsystem<UMyNetworkSubsystem>();
		if (MyNetworkSystem && MySlot)
		{
			uint8_t player_state = (m_bReadyToPlay) ? (PACKET_DATA::SessionPlayer::STATE_IN_ROOM::READY) : (PACKET_DATA::SessionPlayer::STATE_IN_ROOM::NOT_READY);
			MyNetworkSystem->RequestUpdatePlayerState(player_state);
		}
	}
}

void URoomWidget::OnLeaveRoom()
{
	if (m_bReadyToPlay == false)
	{
		UMyNetworkSubsystem* MyNetworkSystem = GetGameInstance()->GetSubsystem<UMyNetworkSubsystem>();
		if (MyNetworkSystem)
		{
			MyNetworkSystem->RequestUpdatePlayerState(PACKET_DATA::SessionPlayer::STATE_IN_ROOM::NONE);
		}
	}
}