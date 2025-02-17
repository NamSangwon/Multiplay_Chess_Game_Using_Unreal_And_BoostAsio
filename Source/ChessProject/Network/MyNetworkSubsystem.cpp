// Fill out your copyright notice in the Description page of Project Settings.

#include "MyNetworkSubsystem.h"

#include "Common/TcpSocketBuilder.h"
#include "SocketSubsystem.h"
#include "Sockets.h"

#include "Buffer.hpp"

#include "SendWorker.h"
#include "RecvWorker.h"

#include <ChessProject/ChessGameInstance.h>
#include <ChessProject/ChessGameMode.h>
#include <ChessProject/Network/NSW_PacketHandler.h>

#include <Kismet/KismetSystemLibrary.h>

void UMyNetworkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	RecvWorkerThread = nullptr;
	SendWorkerThread = nullptr;

	m_Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("ToServerSocket"), false);

	UChessGameInstance* ChessGameInstance = Cast<UChessGameInstance>(GetGameInstance());
	if (ChessGameInstance && m_Socket)
	{
		ConnectToServer(TEXT("192.168.8.41"), 31400);
	}
	else
	{
		UKismetSystemLibrary::QuitGame(this, 0, EQuitPreference::Quit, false);
	}
}

void UMyNetworkSubsystem::Deinitialize()
{
	DisconnectToServer();
}

void UMyNetworkSubsystem::ConnectToServer(FString Address, int port)
{
	if (m_Socket)
	{
		FIPv4Address ip;
		FIPv4Address::Parse(Address, ip);
		TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

		InternetAddr->SetIp(ip.Value);
		InternetAddr->SetPort(port);

		if (InternetAddr->IsValid())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Connecting To Server..."));

			if (m_Socket->Connect(*InternetAddr))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Connection Success"));
				Run();
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Connection Failed"));
				DisconnectToServer();
			}
		}
	}
}

void UMyNetworkSubsystem::DisconnectToServer()
{
	if (m_Socket)
	{
		Disconnect();

		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get();
		if (SocketSubsystem)
		{
			SocketSubsystem->DestroySocket(m_Socket);
			m_Socket = nullptr;
		}
	}
}

void UMyNetworkSubsystem::Run()
{
	if (m_Socket)
	{
		AsyncTask(
			ENamedThreads::AnyThread,
			[this]() {
				SendWorkerThread = MakeShared<FSendWorker>(m_Socket);
			}
		);

		AsyncTask(
			ENamedThreads::AnyThread,
			[this]() {
				RecvWorkerThread = MakeShared<FRecvWorker>(m_Socket);
			}
		);
	}
}

void UMyNetworkSubsystem::Disconnect()
{
	if (RecvWorkerThread)
	{
		RecvWorkerThread->Stop();
	}

	if (SendWorkerThread)
	{
		SendWorkerThread->Stop();
	}
}

bool UMyNetworkSubsystem::IsConnected() const
{
	if (m_Socket)
	{
		return (m_Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected);
	}

	return false;
}

void UMyNetworkSubsystem::RequestProgramStart(const FString PlayerID)
{
	if (SendWorkerThread.IsValid())
	{
		if (!PlayerID.IsEmpty())
		{
			std::string player_id = TCHAR_TO_ANSI(*PlayerID);

			Buffer SendBuffer;
			NSW_PacketHandler::GetInstance().PackPlayerInfoPacket(SendBuffer, PACKET_ID::PT_PROGRAM_START, player_id, static_cast<uint8_t>(GAME_STATE::INIT));
			SendWorkerThread->WritePacketQueue(SendBuffer);
		}
	}
}

void UMyNetworkSubsystem::RequestCreateRoom(const FString RoomName)
{
	std::string room_name;
	if (RoomName.IsEmpty())
	{
		UChessGameInstance* ChessGameInstance = Cast<UChessGameInstance>(GetGameInstance());
		if (ChessGameInstance)
		{
			FString strRoomName = FString::Printf(TEXT("%s's Chess Game Room"), *ChessGameInstance->GetPlayerID().ToString());
			room_name = TCHAR_TO_ANSI(*strRoomName);
		}
	}
	else
	{
		room_name = TCHAR_TO_ANSI(*RoomName);
	}

	if (SendWorkerThread.IsValid())
	{
		if (!room_name.empty())
		{
			Buffer SendBuffer;
			NSW_PacketHandler::GetInstance().PackRoomInfoPacket(SendBuffer, PACKET_ID::PT_CREATE_ROOM, 0, room_name, 0);
			SendWorkerThread->WritePacketQueue(SendBuffer);
		}
	}
}

void UMyNetworkSubsystem::RequestEnterRoom(const uint8_t RoomIndex)
{
	if (SendWorkerThread.IsValid())
	{
		Buffer SendBuffer;
		NSW_PacketHandler::GetInstance().PackRoomInfoPacket(SendBuffer, PACKET_ID::PT_ENTER_ROOM, RoomIndex, std::string(), 0);
		SendWorkerThread->WritePacketQueue(SendBuffer);
	}
}

void UMyNetworkSubsystem::RequestUpdatePlayerState(const uint8_t player_state)
{
	if (SendWorkerThread.IsValid())
	{
		UChessGameInstance* ChessGameInstance = Cast<UChessGameInstance>(GetGameInstance());
		if (ChessGameInstance)
		{
			std::string player_name = TCHAR_TO_ANSI(*ChessGameInstance->GetPlayerID().ToString());

			if (!player_name.empty())
			{
				Buffer SendBuffer;
				NSW_PacketHandler::GetInstance().PackPlayerInfoPacket(SendBuffer, PACKET_ID::PT_UPDATE_PLAYER_STATE, player_name, player_state);
				SendWorkerThread->WritePacketQueue(SendBuffer);
			}
		}
	}
}

void UMyNetworkSubsystem::RequestUpdateRoom()
{
	if (SendWorkerThread.IsValid())
	{
		Buffer SendBuffer;
		NSW_PacketHandler::GetInstance().PackRequestPacket(SendBuffer, PACKET_ID::PT_REQ_PLAYER_LIST);
		SendWorkerThread->WritePacketQueue(SendBuffer);
	}
}

void UMyNetworkSubsystem::RequestUpdateLobby()
{
	if (SendWorkerThread.IsValid())
	{
		Buffer SendBuffer;
		NSW_PacketHandler::GetInstance().PackRequestPacket(SendBuffer, PACKET_ID::PT_REQ_ROOM_LIST);
		SendWorkerThread->WritePacketQueue(SendBuffer);
	}
}

void UMyNetworkSubsystem::RequestChessMove(const uint8_t src_slot_index, const uint8_t dest_slot_index)
{
	if (SendWorkerThread.IsValid())
	{
		UChessGameInstance* ChessGameInstance = Cast<UChessGameInstance>(GetGameInstance());
		AChessGameMode* ChessGameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
		if (ChessGameInstance && ChessGameMode)
		{
			int round = ChessGameMode->GetRound();

			Buffer SendBuffer;
			NSW_PacketHandler::GetInstance().PackChessMovePacket(SendBuffer, PACKET_ID::PT_CHESS_MOVE, round, src_slot_index, dest_slot_index);
			SendWorkerThread->WritePacketQueue(SendBuffer);

			ChessGameInstance->ToggleTurn(false);
		}
	}
}

void UMyNetworkSubsystem::RequestChessCastling(const uint8_t king_slot_index, const uint8_t rook_slot_index)
{
	if (SendWorkerThread.IsValid())
	{
		UChessGameInstance* ChessGameInstance = Cast<UChessGameInstance>(GetGameInstance());
		AChessGameMode* ChessGameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
		if (ChessGameInstance && ChessGameMode)
		{
			int round = ChessGameMode->GetRound();

			Buffer SendBuffer;
			NSW_PacketHandler::GetInstance().PackChessCastlingPacket(SendBuffer, PACKET_ID::PT_CHESS_CASTLING, round, king_slot_index, rook_slot_index);
			SendWorkerThread->WritePacketQueue(SendBuffer);

			ChessGameInstance->ToggleTurn(false);
		}
	}
}

void UMyNetworkSubsystem::RequestChessPromotion(const uint8_t pawn_slot_index, const uint8_t promotion_type)
{
	if (SendWorkerThread.IsValid())
	{
		UChessGameInstance* ChessGameInstance = Cast<UChessGameInstance>(GetGameInstance());
		AChessGameMode* ChessGameMode = Cast<AChessGameMode>(GetWorld()->GetAuthGameMode());
		if (ChessGameInstance && ChessGameMode)
		{
			int round = ChessGameMode->GetRound();

			Buffer SendBuffer;
			NSW_PacketHandler::GetInstance().PackChessPromotionPacket(SendBuffer, PACKET_ID::PT_CHESS_PROMOTION, round, pawn_slot_index, promotion_type);
			SendWorkerThread->WritePacketQueue(SendBuffer);

			ChessGameInstance->ToggleTurn(false);
		}
	}
}

void UMyNetworkSubsystem::ResponsePacket()
{
	if (RecvWorkerThread.IsValid())
	{
		Buffer RecvBuffer;
		if (RecvWorkerThread->ReadPacketQueue(RecvBuffer))
		{
			uint16_t content_length; uint32_t pkt_id; uint8_t seq_num;
			NSW_PacketHandler::GetInstance().UnpackHeader(RecvBuffer, content_length, pkt_id, seq_num);

			switch (pkt_id)
			{
			case PACKET_ID::PT_PROGRAM_START:		ResponseProgramStart(RecvBuffer, content_length, seq_num); break;
			case PACKET_ID::PT_CHAT:				ResponseChat(RecvBuffer, content_length, seq_num); break;
			case PACKET_ID::PT_REQ_ROOM_LIST:		break;
			case PACKET_ID::PT_RES_ROOM_LIST:		ResponseUpdateLobby(RecvBuffer, content_length, seq_num); break;
			case PACKET_ID::PT_REQ_PLAYER_LIST:		break;
			case PACKET_ID::PT_RES_PLAYER_LIST:		ResponseUpdateRoom(RecvBuffer, content_length, seq_num); break;
			case PACKET_ID::PT_CREATE_ROOM:			ResponseCreateRoom(RecvBuffer, content_length, seq_num); break;
			case PACKET_ID::PT_ENTER_ROOM:			ResponseEnterRoom(RecvBuffer, content_length, seq_num); break;
			case PACKET_ID::PT_LEAVE_ROOM:			ResponseEnterLobby(RecvBuffer, content_length, seq_num); break;
			case PACKET_ID::PT_UPDATE_PLAYER_STATE:	ResponseUpdatePlayerState(RecvBuffer, content_length, seq_num); break;
			case PACKET_ID::PT_GAME_START:			ResponseGameStart(RecvBuffer, content_length, seq_num); break;
			case PACKET_ID::PT_CHESS_EVENT:			ResponseChessEvent(RecvBuffer, content_length, seq_num); break;
			case PACKET_ID::PT_CHESS_MOVE:			ResponseMovingChessPiece(RecvBuffer, content_length, seq_num); break;
			case PACKET_ID::PT_CHESS_CASTLING:		ResponseCastlingChessPiece(RecvBuffer, content_length, seq_num); break;
			case PACKET_ID::PT_CHESS_PROMOTION:		ResponsePromoteChessPiece(RecvBuffer, content_length, seq_num); break;
			}
		}
	}
}

void UMyNetworkSubsystem::ResponseProgramStart(Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num)
{
	UChessGameInstance* ChessGameInstance = Cast<UChessGameInstance>(GetGameInstance());
	if (ChessGameInstance)
	{
		std::string player_id;
		uint8_t player_state;
		uint16_t pkt_data_length = NSW_PacketHandler::GetInstance().UnpackPlayerInfoPacket(RecvPacket, player_id, player_state);

		if (pkt_data_length == content_length)
		{
			FText PlayerID = FText::FromString(player_id.c_str());
			ChessGameInstance->ProgramStart(PlayerID);
		}
	}
}

void UMyNetworkSubsystem::ResponseChat(Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num)
{
	UChessGameInstance* ChessGameInstance = Cast<UChessGameInstance>(GetGameInstance());
	if (ChessGameInstance)
	{
		std::string chat_msg;
		uint16_t pkt_data_length = NSW_PacketHandler::GetInstance().UnpackChatPacket(RecvPacket, chat_msg);

		if (pkt_data_length == content_length && !chat_msg.empty())
		{
			FString ChatMessage(chat_msg.c_str());
			ChessGameInstance->ShowChat(ChatMessage);
		}
	}
}

void UMyNetworkSubsystem::ResponseUpdateLobby(Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num)
{
	UChessGameInstance* ChessGameInstance = Cast<UChessGameInstance>(GetGameInstance());
	if (ChessGameInstance)
	{
		std::vector<PACKET_DATA::SessionRoom> RoomList;
		uint16_t pkt_data_length = NSW_PacketHandler::GetInstance().UnpackRoomListInfoPacket(RecvPacket, RoomList);

		if (pkt_data_length == content_length)
		{
			if (seq_num == 1)
			{
				ChessGameInstance->PreUpdateLobby();
			}

			ChessGameInstance->UpdateLobby(RoomList);
		}
	}
}

void UMyNetworkSubsystem::ResponseUpdateRoom(Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num)
{
	UChessGameInstance* ChessGameInstance = Cast<UChessGameInstance>(GetGameInstance());
	if (ChessGameInstance)
	{
		std::vector<PACKET_DATA::SessionPlayer> PlayerList;
		uint16_t pkt_data_length = NSW_PacketHandler::GetInstance().UnpackPlayerListInfoPacket(RecvPacket, PlayerList);

		if (pkt_data_length == content_length)
		{
			ChessGameInstance->UpdateRoom(PlayerList);
		}
	}
}

void UMyNetworkSubsystem::ResponseCreateRoom(Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num)
{
	uint8_t room_index = -1;
	std::string room_name;
	uint8_t participants = -1;

	uint16_t pkt_data_length = NSW_PacketHandler::GetInstance().UnpackRoomInfoPacket(RecvPacket, room_index, room_name, participants);

	if (pkt_data_length == content_length)
	{
		if (!room_name.empty() && room_index > 0)
		{
			RequestEnterRoom(room_index);
		}
	}
}

void UMyNetworkSubsystem::ResponseEnterRoom(Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num)
{
	UChessGameInstance* ChessGameInstance = Cast<UChessGameInstance>(GetGameInstance());
	if (ChessGameInstance)
	{
		uint8_t room_index = -1;
		std::string room_name;
		uint8_t participants = -1;

		uint16_t pkt_data_length = NSW_PacketHandler::GetInstance().UnpackRoomInfoPacket(RecvPacket, room_index, room_name, participants);

		if (pkt_data_length == content_length)
		{
			if (room_index > 0 && (participants >= 0 && participants <= 2))
			{
				FText RoomName = FText::FromString(room_name.c_str());
				ChessGameInstance->EnterRoom(RoomName);
			}
		}
	}
}

void UMyNetworkSubsystem::ResponseEnterLobby(Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num)
{
	UChessGameInstance* ChessGameInstance = Cast<UChessGameInstance>(GetGameInstance());
	if (ChessGameInstance)
	{
		ChessGameInstance->EnterLobby();
	}
}

void UMyNetworkSubsystem::ResponseUpdatePlayerState(Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num)
{
	UChessGameInstance* ChessGameInstance = Cast<UChessGameInstance>(GetGameInstance());
	if (ChessGameInstance)
	{
		std::string player_id;
		uint8_t player_state;
		uint16_t pkt_data_length = NSW_PacketHandler::GetInstance().UnpackPlayerInfoPacket(RecvPacket, player_id, player_state);

		if (pkt_data_length == content_length)
		{
			if (player_state == static_cast<uint8_t>(GAME_STATE::IN_ROOM))
			{
				FText PlayerID = FText::FromString(player_id.c_str());

				ChessGameInstance->UpdatePlayerState(PlayerID, player_state);
			}
		}
	}
}

void UMyNetworkSubsystem::ResponseGameStart(Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num)
{
	UChessGameInstance* ChessGameInstance = Cast<UChessGameInstance>(GetGameInstance());
	if (ChessGameInstance)
	{
		uint8_t round;
		std::string white_team_player;
		std::string black_team_player;
		uint16_t pkt_data_length = NSW_PacketHandler::GetInstance().UnpackGameInfoPacket(RecvPacket, round, white_team_player, black_team_player);

		if (pkt_data_length == content_length)
		{
			FText WhiteTeamPlayer = FText::FromString(white_team_player.c_str());
			FText BlackTeamPlayer = FText::FromString(black_team_player.c_str());

			if (!WhiteTeamPlayer.IsEmpty() && !BlackTeamPlayer.IsEmpty())
			{
				ChessGameInstance->GameStart(round, WhiteTeamPlayer, BlackTeamPlayer);
			}
		}
	}
}

void UMyNetworkSubsystem::ResponseChessEvent(Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num)
{
	UChessGameInstance* ChessGameInstance = Cast<UChessGameInstance>(GetGameInstance());
	if (ChessGameInstance)
	{
		uint8_t event_type;
		std::string event_player;
		uint16_t pkt_data_length = NSW_PacketHandler::GetInstance().UnpackChessEventPacket(RecvPacket, event_type, event_player);

		if (pkt_data_length == content_length)
		{
			ChessGameInstance->ProcessChessEvent(event_type, event_player.c_str());
		}
	}
}

void UMyNetworkSubsystem::ResponseMovingChessPiece(Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num)
{
	UChessGameInstance* ChessGameInstance = Cast<UChessGameInstance>(GetGameInstance());
	if (ChessGameInstance)
	{
		uint8_t round = -1;
		uint8_t src_slot_index = -1;
		uint8_t dest_slot_index = -1;
		uint16_t pkt_data_length = NSW_PacketHandler::GetInstance().UnpackChessMovePacket(RecvPacket, round, src_slot_index, dest_slot_index);

		if (pkt_data_length == content_length)
		{
			ChessGameInstance->ProcessChessPieceMove(round, src_slot_index, dest_slot_index);
		}
	}
}

void UMyNetworkSubsystem::ResponseCastlingChessPiece(Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num)
{
	UChessGameInstance* ChessGameInstance = Cast<UChessGameInstance>(GetGameInstance());
	if (ChessGameInstance)
	{
		uint8_t round = -1;
		uint8_t king_slot_index = -1;
		uint8_t rook_slot_index = -1;
		uint16_t pkt_data_length = NSW_PacketHandler::GetInstance().UnpackChessCastlingPacket(RecvPacket, round, king_slot_index, rook_slot_index);

		if (pkt_data_length == content_length)
		{
			ChessGameInstance->ProcessCastling(round, king_slot_index, rook_slot_index);
		}
	}
}

void UMyNetworkSubsystem::ResponsePromoteChessPiece(Buffer& RecvPacket, const uint16_t& content_length, const uint8_t& seq_num)
{
	UChessGameInstance* ChessGameInstance = Cast<UChessGameInstance>(GetGameInstance());
	if (ChessGameInstance)
	{
		uint8_t round = -1;
		uint8_t pawn_slot_index = -1;
		uint8_t promotion_type = -1;
		uint16_t pkt_data_length = NSW_PacketHandler::GetInstance().UnpackChessPromotionPacket(RecvPacket, round, pawn_slot_index, promotion_type);

		if (pkt_data_length == content_length)
		{
			ChessGameInstance->ProcessPromotion(round, pawn_slot_index, promotion_type);
		}
	}
}