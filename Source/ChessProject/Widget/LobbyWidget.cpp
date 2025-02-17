// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"

#include "Components/ScrollBox.h"
#include "Components/Button.h"

#include <ChessProject/Network/MyNetworkSubsystem.h>
#include <ChessProject/Widget/RoomSlotWidget.h>

#include <Kismet/KismetSystemLibrary.h>

void ULobbyWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RoomListBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("RoomListBox")));

	CreateRoomButton = Cast<UButton>(GetWidgetFromName(TEXT("CreateRoomButton")));
	if (CreateRoomButton)
	{
		CreateRoomButton->OnClicked.AddUniqueDynamic(this, &ULobbyWidget::OnCreateRoom);
	}

	ProgramEndButton = Cast<UButton>(GetWidgetFromName(TEXT("ProgramEndButton")));
	if (ProgramEndButton)
	{
		ProgramEndButton->OnClicked.AddUniqueDynamic(this, &ULobbyWidget::OnProgramEnd);
	}

	UpdateLobbyButton = Cast<UButton>(GetWidgetFromName(TEXT("UpdateLobbyButton")));
	if (UpdateLobbyButton)
	{
		UpdateLobbyButton->OnClicked.AddUniqueDynamic(this, &ULobbyWidget::OnUpdateLobby);
	}
}

void ULobbyWidget::ClearRoomList()
{
	RoomListBox->ClearChildren();
}

void ULobbyWidget::AppendRoom(const int room_index, const std::string room_name, const int room_participants)
{
	if (RoomListBox)
	{
		if (RoomSlotWidgetClass)
		{
			URoomSlotWidget* NewRoomSlot = CreateWidget<URoomSlotWidget>(this, RoomSlotWidgetClass);
			if (NewRoomSlot)
			{
				NewRoomSlot->SetIndex(room_index);
				NewRoomSlot->SetName(FText::FromString(room_name.c_str()));
				NewRoomSlot->SetParticipants(room_participants);

				RoomListBox->AddChild(NewRoomSlot);
			}
		}
	}
}

void ULobbyWidget::OnCreateRoom()
{
	UMyNetworkSubsystem* MyNetworkSystem = GetGameInstance()->GetSubsystem<UMyNetworkSubsystem>();
	if (MyNetworkSystem)
	{
		MyNetworkSystem->RequestCreateRoom(TEXT(""));
	}
}

void ULobbyWidget::OnProgramEnd()
{
	UKismetSystemLibrary::QuitGame(this, 0, EQuitPreference::Quit, false);
}

void ULobbyWidget::OnUpdateLobby()
{
	UMyNetworkSubsystem* MyNetworkSystem = GetGameInstance()->GetSubsystem<UMyNetworkSubsystem>();
	if (MyNetworkSystem)
	{
		MyNetworkSystem->RequestUpdateLobby();
	}
}
