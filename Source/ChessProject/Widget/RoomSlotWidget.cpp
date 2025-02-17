// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomSlotWidget.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"

#include <ChessProject/Network/MyNetworkSubsystem.h>

void URoomSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RoomIndex = Cast<UTextBlock>(GetWidgetFromName(TEXT("RoomIndex")));
	RoomName = Cast<UTextBlock>(GetWidgetFromName(TEXT("RoomName")));
	RoomParticipants = Cast<UTextBlock>(GetWidgetFromName(TEXT("RoomParticipants")));

	RoomButton = Cast<UButton>(GetWidgetFromName(TEXT("RoomButton")));
	if (RoomButton)
	{
		RoomButton->OnClicked.AddUniqueDynamic(this, &URoomSlotWidget::OnEnterRoom);
	}
}

void URoomSlotWidget::SetIndex(const int room_index)
{
	if (RoomIndex)
	{
		RoomIndex->SetText(FText::AsNumber(room_index));
	}
}

void URoomSlotWidget::SetName(const FText room_name)
{
	if (RoomName)
	{
		RoomName->SetText(room_name);
	}
}

void URoomSlotWidget::SetParticipants(const int room_participants)
{
	FString str_participant = FString::Printf(TEXT("%d / 2"), room_participants); // room_participants + TEXT(" / 2");

	if (RoomParticipants)
	{
		RoomParticipants->SetText(FText::FromString(str_participant));
	}
}

void URoomSlotWidget::OnEnterRoom()
{
	UMyNetworkSubsystem* MyNetworkSystem = GetGameInstance()->GetSubsystem<UMyNetworkSubsystem>();
	if (MyNetworkSystem)
	{
		FText txt_room_index = RoomIndex->GetText();
		uint8_t room_index = FCString::Atoi(*txt_room_index.ToString());
	
		FText txt_room_name = RoomName->GetText();

		FText txt_room_participants = RoomParticipants->GetText();
		uint8_t room_participants = FCString::Atoi(*txt_room_participants.ToString());

		if (room_index > 0 && (room_participants >= 0 && room_participants < 2))
		{
			MyNetworkSystem->RequestEnterRoom(room_index);
		}
	}
}
