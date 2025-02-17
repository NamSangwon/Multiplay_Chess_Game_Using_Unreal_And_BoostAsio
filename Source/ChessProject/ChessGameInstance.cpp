// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessGameInstance.h"

#include <Components/TextBlock.h>

#include <ChessProject/ChessGameMode.h>
#include <ChessProject/Player/ChessPlayer.h>
#include <ChessProject/Network/MyNetworkSubsystem.h>

#include <ChessProject/Widget/ProgramStartWidget.h>
#include <ChessProject/Widget/LobbyWidget.h>
#include <ChessProject/Widget/PromotionWidget.h>
#include <ChessProject/Widget/RoomWidget.h>
#include <ChessProject/Widget/InGameWidget.h>

#include <Kismet/GameplayStatics.h>

void UChessGameInstance::Init()
{
	Super::Init();

	m_SessionState = GAME_STATE::INIT;

	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UChessGameInstance::Tick));
}

void UChessGameInstance::Shutdown()
{
	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);

	Super::Shutdown();
}

bool UChessGameInstance::Tick(float DeltaSeconds)
{
	UMyNetworkSubsystem* MyNetworkSystem = GetSubsystem<UMyNetworkSubsystem>();
	if (MyNetworkSystem)
	{
		if (MyNetworkSystem->IsConnected())
		{
			MyNetworkSystem->ResponsePacket();
		}
		else
		{
			UKismetSystemLibrary::QuitGame(this, 0, EQuitPreference::Quit, false);
		}
	}

	AChessPlayer* MyPlayer = Cast<AChessPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	// My Turn & Not Turn End
	if (MyPlayer->IsMyTurn() && m_eTurnState != TURN_STATE::END)
	{
		// Countdown
		m_fCurrTimerSeconds -= DeltaSeconds;

		// Set Time to Widget
		if (m_InGameWidget)
		{
			int curr_seconds = FMath::RoundToInt(m_fCurrTimerSeconds);
			int countdown_seconds = FMath::Clamp(curr_seconds, 0, int(StartTimerSeconds));
			m_InGameWidget->UpdateCountdown(countdown_seconds);
		}

		// if over time
		if (m_fCurrTimerSeconds <= 0)
		{
			// Auto Play this turn (Random Move)
			MyPlayer->DoRandomMove();
			ToggleTurn(false);
		}
	}

	return true;
}

void UChessGameInstance::ProgramStart(const FText PlayerID)
{
	if (m_SessionState == GAME_STATE::INIT)
	{
		SetPlayerID(PlayerID);

		if (m_PlayerID.IsEmpty() == false)
		{
			HideAllWidget();
			ShowLobbyWidget();
		}
	}
}

void UChessGameInstance::ShowChat(const FString ChatMessage)
{
	if (ChatMessage.IsEmpty() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CHAT] - %s"), *ChatMessage);
	}
}

void UChessGameInstance::PreUpdateLobby()
{
	if (m_SessionState == GAME_STATE::IN_LOBBY)
	{
		if (m_LobbyWidget)
		{
			m_LobbyWidget->ClearRoomList();
		}
	}
}

void UChessGameInstance::UpdateLobby(const std::vector<PACKET_DATA::SessionRoom>& RoomList)
{
	if (m_SessionState != GAME_STATE::IN_LOBBY)
	{
		AChessGameMode* ChessGameMode = Cast<AChessGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (ChessGameMode)
		{
			ChessGameMode->GameEnd();
		}

		HideAllWidget();
		ShowLobbyWidget();
	}

	for (auto& room : RoomList)
	{
		m_LobbyWidget->AppendRoom(room.index, room.name, room.participant_num);
	}
}

void UChessGameInstance::UpdateRoom(const std::vector<PACKET_DATA::SessionPlayer>& PlayerList)
{
	if (m_SessionState != GAME_STATE::IN_ROOM)
	{
		AChessGameMode* ChessGameMode = Cast<AChessGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (ChessGameMode)
		{
			ChessGameMode->GameEnd();
		}

		HideAllWidget();
		ShowRoomWidget();
	}
	
	if (m_RoomWidget)
	{
		m_RoomWidget->SetMySlot(FText::GetEmpty(), false);
		m_RoomWidget->SetOppositeSlot(FText::GetEmpty(), false);

		for (auto& player : PlayerList)
		{
			FText PlayerID = FText::FromString(player.id);

			UpdatePlayerState(PlayerID, player.state);
		}
	}
}

void UChessGameInstance::EnterRoom(const FText RoomName)
{
	if (m_SessionState == GAME_STATE::IN_LOBBY || m_SessionState == GAME_STATE::IN_GAME)
	{
		if (!RoomName.IsEmpty())
		{
			HideAllWidget();
			ShowRoomWidget();

			if (m_RoomWidget)
			{
				m_RoomWidget->SetRoomName(RoomName);
			}
		}
	}
}

void UChessGameInstance::EnterLobby()
{
	if (m_SessionState == GAME_STATE::IN_ROOM)
	{
		HideAllWidget();
		ShowLobbyWidget();
	}
}

void UChessGameInstance::UpdatePlayerState(const FText PlayerID, const uint8_t PlayerState)
{
	if (m_SessionState == GAME_STATE::IN_ROOM)
	{
		UMyNetworkSubsystem* MyNetworkSystem = GetSubsystem<UMyNetworkSubsystem>();
		if (MyNetworkSystem)
		{
			if (m_RoomWidget)
			{
				if (m_PlayerID.EqualTo(PlayerID))
				{
					switch (PlayerState)
					{
					case PACKET_DATA::SessionPlayer::STATE_IN_ROOM::NONE:		MyNetworkSystem->RequestUpdatePlayerState(PACKET_DATA::SessionPlayer::STATE_IN_ROOM::NONE); break;
					case PACKET_DATA::SessionPlayer::STATE_IN_ROOM::NOT_READY:	m_RoomWidget->SetMySlot(PlayerID, false); break;
					case PACKET_DATA::SessionPlayer::STATE_IN_ROOM::READY:		m_RoomWidget->SetMySlot(PlayerID, true); break;
					}
				}
				else
				{
					switch (PlayerState)
					{
					case PACKET_DATA::SessionPlayer::STATE_IN_ROOM::NONE:		m_RoomWidget->SetOppositeSlot(FText::GetEmpty(), false); break;
					case PACKET_DATA::SessionPlayer::STATE_IN_ROOM::NOT_READY:	m_RoomWidget->SetOppositeSlot(PlayerID, false); break;
					case PACKET_DATA::SessionPlayer::STATE_IN_ROOM::READY:		m_RoomWidget->SetOppositeSlot(PlayerID, true); break;
					}
				}
			}
		}
	}
}

void UChessGameInstance::GameStart(const uint8_t GameRound, const FText WhiteTeamPlayer, const FText BlackTeamPlayer)
{
	if (m_SessionState == GAME_STATE::IN_ROOM)
	{
		UMyNetworkSubsystem* MyNetworkSystem = GetSubsystem<UMyNetworkSubsystem>();
		if (MyNetworkSystem)
		{
			if (GameRound == 1)
			{
				AChessGameMode* ChessGameMode = Cast<AChessGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
				if (ChessGameMode)
				{
					HideAllWidget();
					ShowInGameWidget();

					if (m_InGameWidget)
					{
						m_InGameWidget->SetPlayerToWhiteSlot(WhiteTeamPlayer);
						m_InGameWidget->SetPlayerToBlackSlot(BlackTeamPlayer);

						bool bFirst = m_PlayerID.EqualTo(WhiteTeamPlayer);
						ChessGameMode->GameStart(bFirst);

						UpdateRound(GameRound);
					}
				}
			}
		}
	}
}

void UChessGameInstance::UpdateRound(int round)
{
	if (m_SessionState == GAME_STATE::IN_GAME)
	{
		AChessGameMode* ChessGameMode = Cast<AChessGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (ChessGameMode)
		{
			if (round > ChessGameMode->GetRound())
			{
				ChessGameMode->NextRound(round);

				if (m_InGameWidget)
				{
					m_InGameWidget->SetRoundText(round);
					m_InGameWidget->SetTurnMark(round);
				}
			}

			AChessPlayer* MyPlayer = Cast<AChessPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
			if (MyPlayer)
			{
				if (MyPlayer->IsMyTurn())
				{
					ToggleTurn(true);
				}
			}
		}
	}
}

void UChessGameInstance::ProcessChessEvent(const uint8_t event_type, const FString event_player)
{
	if (m_SessionState == GAME_STATE::IN_GAME)
	{
		if (m_InGameWidget)
		{
			FText game_state = FText::GetEmpty();
			FText game_result = FText::FromString(FString::Printf(TEXT("Winner is %s!"), *event_player));

			bool bCheckState = false;
			switch (event_type)
			{
			case PACKET_DATA::CHESS_EVENT::EVENT_TYPE::CHECK:
				bCheckState = true;
				break;
			case PACKET_DATA::CHESS_EVENT::EVENT_TYPE::CHECKMATE:
				bCheckState = false;
				game_state = FText::FromString(TEXT("Checkmate"));
				GameEnd(game_state, game_result);
				ToggleTurn(false);
				break;
			case PACKET_DATA::CHESS_EVENT::EVENT_TYPE::STALEMATE:
				bCheckState = false;
				game_state = FText::FromString(TEXT("Stalemate"));
				GameEnd(game_state, game_result);
				ToggleTurn(false);
				break;
			default:
				bCheckState = false;
				break;
			}

			m_InGameWidget->ShowCheckState(bCheckState);
		}
	}
}

void UChessGameInstance::GameEnd(const FText game_state, const FText game_result)
{
	if (m_SessionState == GAME_STATE::IN_GAME)
	{
		ShowGameResultWidget(game_state, game_result);

		AChessGameMode* ChessGameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());		
		if (ChessGameMode)
		{
			ChessGameMode->GamePause(true);
		}

		FTimerHandle myTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			myTimerHandle, 
			FTimerDelegate::CreateLambda(
				[&]() {
					UMyNetworkSubsystem* MyNetworkSystem = GetSubsystem<UMyNetworkSubsystem>();
					AChessGameMode* ChessGameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
					if (MyNetworkSystem && ChessGameMode)
					{
						ChessGameMode->GameEnd();

						HideAllWidget();
						ShowRoomWidget();

						MyNetworkSystem->RequestUpdateRoom();
					}
				}
			),
			5.0f,
			false
		); 
	}
}

void UChessGameInstance::ProcessChessPieceMove(const uint8_t round, const uint8_t src_slot_index, const uint8_t dest_slot_index)
{
	if (m_SessionState == GAME_STATE::IN_GAME)
	{
		UMyNetworkSubsystem* MyNetworkSystem = GetSubsystem<UMyNetworkSubsystem>();
		if (MyNetworkSystem)
		{
			if ((0 <= src_slot_index && src_slot_index < 64) && (0 <= dest_slot_index && dest_slot_index < 64))
			{
				AChessPlayer* MyPlayer = Cast<AChessPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
				if (MyPlayer)
				{
					MyPlayer->DoMoveChessPiece(src_slot_index, dest_slot_index);
					UpdateRound(round);
				}
			}
		}
	}
}

void UChessGameInstance::ProcessCastling(const uint8_t round, const uint8_t king_slot_index, const uint8_t rook_slot_index)
{
	if (m_SessionState == GAME_STATE::IN_GAME)
	{
		UMyNetworkSubsystem* MyNetworkSystem = GetSubsystem<UMyNetworkSubsystem>();
		if (MyNetworkSystem)
		{
			if ((0 <= king_slot_index && king_slot_index < 64) && (0 <= rook_slot_index && rook_slot_index < 64))
			{
				AChessPlayer* MyPlayer = Cast<AChessPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
				if (MyPlayer)
				{
					MyPlayer->DoCastling(king_slot_index, rook_slot_index);
					UpdateRound(round);
				}
			}
		}
	}
}

void UChessGameInstance::ProcessPromotion(const uint8_t round, const uint8_t pawn_slot_index, const uint8_t promotion_type)
{
	if (m_SessionState == GAME_STATE::IN_GAME)
	{
		UMyNetworkSubsystem* MyNetworkSystem = GetSubsystem<UMyNetworkSubsystem>();
		if (MyNetworkSystem)
		{
			if ((0 <= pawn_slot_index && pawn_slot_index < 64) && (CHESSPIECE_TYPE::ROOK <= promotion_type && promotion_type <= CHESSPIECE_TYPE::QUEEN))
			{
				AChessPlayer* MyPlayer = Cast<AChessPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
				if (MyPlayer)
				{
					MyPlayer->DoPromotion(pawn_slot_index, promotion_type);
					UpdateRound(round);
				}
			}
		}
	}
}

void UChessGameInstance::ProgramEnd()
{
	HideAllWidget();
}

void UChessGameInstance::HideAllWidget()
{
	if (m_ProgramStartWidget)
	{
		m_ProgramStartWidget->RemoveFromParent();
		m_ProgramStartWidget = nullptr;
	}
	if (m_LobbyWidget)
	{
		m_LobbyWidget->RemoveFromParent();
		m_LobbyWidget = nullptr;
	}
	if (m_RoomWidget)
	{
		m_RoomWidget->RemoveFromParent();
		m_RoomWidget = nullptr;
	}
	if (m_InGameWidget)
	{
		m_InGameWidget->RemoveFromParent();
		m_InGameWidget = nullptr;
	}
	if (GameResultWidget)
	{
		GameResultWidget->RemoveFromParent();
		GameResultWidget = nullptr;
	}
}

void UChessGameInstance::ShowProgramStartWidget()
{
	if (ProgramStartWidgetClass)
	{
		m_ProgramStartWidget = Cast<UProgramStartWidget>(CreateWidget(GetWorld(), ProgramStartWidgetClass));
		if (m_ProgramStartWidget)
		{
			m_ProgramStartWidget->AddToViewport();
			m_SessionState = GAME_STATE::INIT;
		}
	}
}

void UChessGameInstance::ShowLobbyWidget()
{
	if (LobbyWidgetClass)
	{
		m_LobbyWidget = Cast<ULobbyWidget>(CreateWidget(GetWorld(), LobbyWidgetClass));
		if (m_LobbyWidget)
		{
			m_LobbyWidget->AddToViewport();
			m_SessionState = GAME_STATE::IN_LOBBY;
		}
	}
}

void UChessGameInstance::ShowRoomWidget()
{
	if (RoomWidgetClass)
	{
		m_RoomWidget = Cast<URoomWidget>(CreateWidget(GetWorld(), RoomWidgetClass));
		if (m_RoomWidget)
		{
			m_RoomWidget->AddToViewport();
			m_SessionState = GAME_STATE::IN_ROOM;
		}
	}
}

void UChessGameInstance::ShowInGameWidget()
{
	if (InGameWidgetClass)
	{
		m_InGameWidget = Cast<UInGameWidget>(CreateWidget(GetWorld(), InGameWidgetClass));
		if (m_InGameWidget)
		{
			m_InGameWidget->AddToViewport();
			m_SessionState = GAME_STATE::IN_GAME;
		}
	}
}

void UChessGameInstance::ShowGameResultWidget(const FText game_state, const FText game_result)
{
	if (GameResultWidgetClass)
	{
		GameResultWidget = CreateWidget(GetWorld(), GameResultWidgetClass);
		if (GameResultWidget)
		{
			UTextBlock* GameState = Cast<UTextBlock>(GameResultWidget->GetWidgetFromName(TEXT("GameState")));
			UTextBlock* GameResult = Cast<UTextBlock>(GameResultWidget->GetWidgetFromName(TEXT("GameResult")));
			if (GameState && GameResult)
			{
				GameState->SetText(game_state);
				GameResult->SetText(game_result);

				GameResultWidget->AddToViewport();
			}
		}
	}
}

void UChessGameInstance::ToggleTurn(bool bToggle)
{
	if (m_SessionState == GAME_STATE::IN_GAME)
	{
		if (m_InGameWidget)
		{
			m_InGameWidget->ShowCountdown(bToggle);
		}

		if (bToggle)
		{
			m_eTurnState = TURN_STATE::START;
			m_fCurrTimerSeconds = StartTimerSeconds;
		}
		else
		{
			m_eTurnState = TURN_STATE::END;
		}
	}
}

void UChessGameInstance::ShowPromotionWidget()
{
	if (PromotionWidgetClass)
	{
		UUserWidget* PromotionWidget = CreateWidget(GetWorld(), PromotionWidgetClass);
		if (PromotionWidget)
		{
			PromotionWidget->AddToViewport();
		}
	}
}
