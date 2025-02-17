#include "Session.h"

#include "NSW_PacketHandler.h"

#include "ChessGameManager.h"
#include "Server.h"
#include "Room.h"

void Session::ASyncRead()
{
	if (m_bDisconnected.load()) return;

	m_Socket.async_read_some(
		boost::asio::buffer(m_cBuffer),
		boost::asio::bind_executor(
			m_Strand,
			boost::bind(
				&Session::HandleRead,
				this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		)
	);
}

void Session::ASyncWrite(Buffer& buf)
{
	if (m_bDisconnected.load()) return;

	buf.GetBuffer(m_cBuffer);

	m_Socket.async_write_some(
		boost::asio::buffer(m_cBuffer),
		boost::asio::bind_executor(
			m_Strand,
			boost::bind(
				&Session::HandleWrite,
				this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		)
	);
}

void Session::Disconnect()
{
	if (m_cServer.lock() != nullptr)
	{
		if (m_bDisconnected.exchange(true)) return;

		if (m_cRoom.lock() != nullptr)
		{
			// Leave Room
			m_cRoom.lock()->Leave(shared_from_this());

			// Free Room Reference
			m_cRoom.reset();
		}

		// Disconnecting
		if (m_Socket.is_open())
		{
			std::cout << "{" << this << "} is closing." << std::endl;
			m_Socket.cancel();
			m_Socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
			m_Socket.close();
		}

		// Delete this Session in server
		m_cServer.lock()->CloseSession(shared_from_this());
	}
}

void Session::HandleRead(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (m_bDisconnected.load()) return;

	if (error)
	{
		if (error == boost::asio::error::eof)
			std::cout << "[" << boost::this_thread::get_id() << "] {" << this << "} 클라이언트와 연결이 끊어졌습니다" << std::endl;
		else
			std::cout << "[" << boost::this_thread::get_id() << "]" << "error No: " << error.value() << " error Message: " << error.message() << std::endl;

		Disconnect();
	}
	else
	{
		ResponsePacket();
			
		memset(m_cBuffer, '\0', sizeof(m_cBuffer));

		ASyncRead();
	}
}

void Session::HandleWrite(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (m_bDisconnected.load()) return;

	if (error)
	{
		if (error == boost::asio::error::eof)
			std::cout << "[" << boost::this_thread::get_id() << "] {" << this << "} 클라이언트와 연결이 끊어졌습니다" << std::endl;
		else
			std::cout << "[" << boost::this_thread::get_id() << "]" << "error No: " << error.value() << " error Message: " << error.message() << std::endl;

		Disconnect();
	}
	else
	{
		memset(m_cBuffer, '\0', sizeof(m_cBuffer));

		ASyncRead();
	}
}

bool Session::EnterRoom(const uint8_t& room_index)
{
	if (m_cServer.lock() != nullptr)
	{
		std::shared_ptr<Room> enter_room = m_cServer.lock()->GetRoom(room_index);

		if (enter_room)
		{
			if (enter_room->GetRoomIndex() == 0 || enter_room->GetParticipantCount() < 2)
			{
				if (m_cRoom.lock() != nullptr)
				{
					m_cRoom.lock()->Leave(shared_from_this());
				}

				m_cRoom = enter_room;
				m_cRoom.lock()->Enter(shared_from_this());

				m_cServer.lock()->DeleteRooms();

				return true;
			}
		}
	}

	return false;
}

bool Session::LeaveRoom()
{
	if (m_cServer.lock() != nullptr)
	{
		std::shared_ptr<Room> lobby = m_cServer.lock()->GetRoom(0);

		if (lobby)
		{
			if (m_cRoom.lock() != nullptr)
			{
				m_cRoom.lock()->Leave(shared_from_this());
			}

			m_cRoom = lobby;
			m_cRoom.lock()->Enter(shared_from_this());

			m_cServer.lock()->DeleteRooms();

			return true;
		}
	}

	return false;
}

bool Session::ResponsePacket()
{
	Buffer RecvBuffer;
	RecvBuffer.SetBuffer(m_cBuffer);

	if (!RecvBuffer.IsNull())
	{
		uint16_t content_length;  uint32_t pkt_id; uint8_t seq_num;
		NSW_PacketHandler::GetInstance().UnpackHeader(RecvBuffer, content_length, pkt_id, seq_num);

		std::cout << "[" << boost::this_thread::get_id() << "] Receive [" << pkt_id << "] PACKET From {" << this << "}" << std::endl;

		switch (pkt_id)
		{
		case PACKET_ID::PT_PROGRAM_START:		return ResponseProgramStart(RecvBuffer, content_length);
		case PACKET_ID::PT_CHAT:				return ResponseChat(RecvBuffer, content_length);
		case PACKET_ID::PT_REQ_ROOM_LIST:		return ResponseUpdateLobby(RecvBuffer, content_length);
		case PACKET_ID::PT_RES_ROOM_LIST:		return false;
		case PACKET_ID::PT_REQ_PLAYER_LIST:		return ResponseUpdateRoom(RecvBuffer, content_length);
		case PACKET_ID::PT_RES_PLAYER_LIST:		return false;
		case PACKET_ID::PT_CREATE_ROOM:			return ResponseCreateRoom(RecvBuffer, content_length);
		case PACKET_ID::PT_ENTER_ROOM:			return ResponseEnterRoom(RecvBuffer, content_length);
		case PACKET_ID::PT_LEAVE_ROOM:			return ResponseEnterLobby(RecvBuffer, content_length);
		case PACKET_ID::PT_UPDATE_PLAYER_STATE:	return ResponseUpdatePlayerState(RecvBuffer, content_length);
		case PACKET_ID::PT_GAME_START:			return false;
		case PACKET_ID::PT_CHESS_EVENT:			return false;
		case PACKET_ID::PT_CHESS_MOVE:			return ResponseMovingChessPiece(RecvBuffer, content_length);
		case PACKET_ID::PT_CHESS_CASTLING:		return ResponseCastlingChessPiece(RecvBuffer, content_length);
		case PACKET_ID::PT_CHESS_PROMOTION:		return ResponsePromoteChessPiece(RecvBuffer, content_length);
		default:								return false;
		}
	}

	return false;
}

bool Session::ResponseProgramStart(Buffer& RecvPacket, uint16_t& content_length)
{
	if (m_cServer.lock() != nullptr)
	{
		std::string player_id;
		uint8_t player_state;
		uint16_t pkt_data_length = NSW_PacketHandler::GetInstance().UnpackPlayerInfoPacket(RecvPacket, player_id, player_state);

		if (pkt_data_length == content_length)
		{
			if (!player_id.empty() && !m_cServer.lock()->IsDuplicatedID(player_id))
			{
				ProcessProgramStart(player_id);

				return true;
			}
		}
	}

	return false;
}

bool Session::ResponseChat(Buffer& RecvPacket, uint16_t& content_length)
{
	if (m_cRoom.lock() != nullptr)
	{
		std::string chat_msg;
		uint16_t pkt_data_length = NSW_PacketHandler::GetInstance().UnpackChatPacket(RecvPacket, chat_msg);

		if (pkt_data_length == content_length)
		{
			if (!chat_msg.empty())
			{
				ProcessChat(chat_msg);

				return true;
			}
		}
	}

	return false;
}

bool Session::ResponseUpdateLobby(Buffer& RecvPacket, uint16_t& content_length)
{
	if (m_cRoom.lock() != nullptr)
	{
		if (m_cRoom.lock()->GetRoomIndex() == 0)
		{
			ProcessUpdateLobby();

			return true;
		}
	}

	return false;
}

bool Session::ResponseUpdateRoom(Buffer& RecvPacket, uint16_t& content_length)
{
	if (m_cRoom.lock() != nullptr)
	{
		uint8_t room_index = m_cRoom.lock()->GetRoomIndex();
		if (room_index > 0)
		{
			ProcessUpdateRoom(room_index);

			return true;
		}
	}

	return false;
}

bool Session::ResponseCreateRoom(Buffer& RecvPacket, uint16_t& content_length)
{
	uint8_t room_index;
	std::string room_name;
	uint8_t room_participants_num;

	uint16_t pkt_data_length = NSW_PacketHandler::GetInstance().UnpackRoomInfoPacket(RecvPacket, room_index, room_name, room_participants_num);

	if (pkt_data_length == content_length)
	{
		if (!room_name.empty())
		{
			ProcessCreateRoom(room_name);

			return true;
		}
	}

	return false;
}

bool Session::ResponseEnterRoom(Buffer& RecvPacket, uint16_t& content_length)
{
	uint8_t room_index;
	std::string room_name;
	uint8_t room_participants_num;

	uint16_t pkt_data_length = NSW_PacketHandler::GetInstance().UnpackRoomInfoPacket(RecvPacket, room_index, room_name, room_participants_num);

	if (pkt_data_length == content_length)
	{
		if (room_index > 0 && (room_participants_num >= 0 && room_participants_num < 2))
		{
			ProcessEnterRoom(room_index);

			return true;
		}
	}

	return false;
}

bool Session::ResponseEnterLobby(Buffer& RecvPacket, uint16_t& content_length)
{
	if (m_cRoom.lock() != nullptr)
	{
		if (m_cRoom.lock()->GetRoomIndex() != 0)
		{
			ProcessLeaveRoom();

			return true;
		}
	}

	return false;
}

bool Session::ResponseUpdatePlayerState(Buffer& RecvPacket, uint16_t& content_length)
{
	std::string player_id;
	uint8_t player_state;
	uint16_t pkt_data_length = NSW_PacketHandler::GetInstance().UnpackPlayerInfoPacket(RecvPacket, player_id, player_state);

	if (pkt_data_length == content_length)
	{
		ProcessUpdatePlayerState(player_id, player_state);

		return true;
	}

	return false;
}

bool Session::ResponseMovingChessPiece(Buffer& RecvPacket, uint16_t& content_length)
{
	if (m_cRoom.lock() != nullptr)
	{
		uint8_t round;
		uint8_t src_slot_index = -1;
		uint8_t dest_slot_index = -1;
		uint16_t pkt_data_length = NSW_PacketHandler::GetInstance().UnpackChessMovePacket(RecvPacket, round, src_slot_index, dest_slot_index);

		if (pkt_data_length == content_length)
		{
			if (m_cChessGameManager.lock() != nullptr)
			{
				ProcessMovingChessPiece(round, src_slot_index, dest_slot_index);

				return true;
			}
		}
	}

	return false;
}

bool Session::ResponseCastlingChessPiece(Buffer& RecvPacket, uint16_t& content_length)
{
	if (m_cRoom.lock() != nullptr)
	{
		uint8_t round;
		uint8_t king_slot_index = -1;
		uint8_t rook_slot_index = -1;
		uint16_t pkt_data_length = NSW_PacketHandler::GetInstance().UnpackChessCastlingPacket(RecvPacket, round, king_slot_index, rook_slot_index);

		if (pkt_data_length == content_length)
		{
			if (m_cChessGameManager.lock() != nullptr)
			{
				ProcessCastlingChessPiece(round, king_slot_index, rook_slot_index);

				return true;
			}
		}
	}

	return false;
}

bool Session::ResponsePromoteChessPiece(Buffer& RecvPacket, uint16_t& content_length)
{
	if (m_cRoom.lock() != nullptr)
	{
		uint8_t round;
		uint8_t pawn_slot_index = -1;
		uint8_t promotion_type = -1;
		uint16_t pkt_data_length = NSW_PacketHandler::GetInstance().UnpackChessPromotionPacket(RecvPacket, round, pawn_slot_index, promotion_type);

		if (pkt_data_length == content_length)
		{
			if (m_cChessGameManager.lock() != nullptr)
			{
				ProcessPromoteChessPiece(round, pawn_slot_index, promotion_type);

				return true;
			}
		}
	}

	return false;
}

void Session::ProcessProgramStart(const std::string id)
{
	if (id.size() <= SESSION_ID_SIZE)
	{
		SetSessionID(id);

		if (!m_SessionID.empty())
		{
			if (EnterRoom(0))
			{
				Buffer buf;
				NSW_PacketHandler::GetInstance().PackPlayerInfoPacket(buf, PACKET_ID::PT_PROGRAM_START, id, PACKET_DATA::SessionPlayer::STATE_IN_ROOM::NONE);
				ASyncWrite(buf);

				ProcessUpdateLobby();
			}
		}
	}
}

void Session::ProcessChat(const std::string chat_msg)
{
	if (m_cRoom.lock() != nullptr) 
	{
		std::string msg = std::format("[ {} ] : {}", m_SessionID, chat_msg);

		if (!msg.empty())
		{
			Buffer SendBuffer;
			NSW_PacketHandler::GetInstance().PackChatPacket(SendBuffer, PACKET_ID::PT_CHAT, msg);
			m_cRoom.lock()->Broadcast(SendBuffer);
		}
	}
}

void Session::ProcessUpdateLobby()
{
	if (m_cServer.lock() != nullptr)
	{
		// Serialize Room List
		Buffer SendBuffer;

		std::vector<PACKET_DATA::SessionRoom> RoomList;
		m_cServer.lock()->GetAllRoom(RoomList);

		std::shared_ptr<Room> Lobby = m_cServer.lock()->GetRoom(0);
		if (Lobby != nullptr)
		{
			int sequence_num = 1;
			int max_send_room_size = PACKET_SIZE / sizeof(PACKET_DATA::SessionRoom);
			std::vector<PACKET_DATA::SessionRoom> SplitRoomList;
			for (int i = 0; i < RoomList.size(); i++)
			{
				if (i >= max_send_room_size)
				{
					NSW_PacketHandler::GetInstance().PackRoomListInfoPacket(SendBuffer, PACKET_ID::PT_RES_ROOM_LIST, sequence_num, SplitRoomList);
					Lobby->Broadcast(SendBuffer);
					SplitRoomList.clear();
					SendBuffer.Clear();
					sequence_num++;
				}

				SplitRoomList.push_back(RoomList[i]);
			}
			NSW_PacketHandler::GetInstance().PackRoomListInfoPacket(SendBuffer, PACKET_ID::PT_RES_ROOM_LIST, sequence_num, SplitRoomList);
			Lobby->Broadcast(SendBuffer);
		}
	}
}

void Session::ProcessUpdateRoom(const uint8_t room_index)
{
	if (m_cServer.lock() != nullptr)
	{		
		// Serialize Room List
		Buffer SendBuffer;

		// Send Player Info In Room
		std::vector<PACKET_DATA::SessionPlayer> PlayerList;
		std::shared_ptr<Room> RoomExistedPlayer = m_cServer.lock()->GetRoom(room_index);
		if (RoomExistedPlayer != nullptr)
		{
			RoomExistedPlayer->GetAllPlayer(PlayerList);

			int sequence_num = 1;
			int max_send_player_size = PACKET_SIZE / sizeof(PACKET_DATA::SessionPlayer);
			std::vector<PACKET_DATA::SessionPlayer> SplitPlayerList;
			for (int i = 0; i < PlayerList.size(); i++)
			{
				if (i >= max_send_player_size)
				{
					NSW_PacketHandler::GetInstance().PackPlayerListInfoPacket(SendBuffer, PACKET_ID::PT_RES_PLAYER_LIST, sequence_num, SplitPlayerList);
					RoomExistedPlayer->Broadcast(SendBuffer);
					SplitPlayerList.clear();
					SendBuffer.Clear();
					sequence_num++;
				}

				SplitPlayerList.push_back(PlayerList[i]);
			}
			NSW_PacketHandler::GetInstance().PackPlayerListInfoPacket(SendBuffer, PACKET_ID::PT_RES_PLAYER_LIST, sequence_num, SplitPlayerList);
			RoomExistedPlayer->Broadcast(SendBuffer);
		}
	}
}

void Session::ProcessCreateRoom(const std::string room_name)
{
	if (m_cServer.lock() != nullptr)
	{
		if (m_cRoom.lock()->GetRoomIndex() == 0)
		{
			int created_room_idx = m_cServer.lock()->CreateRoom(room_name);

			if (created_room_idx > 0)
			{
				Buffer buf;
				NSW_PacketHandler::GetInstance().PackRoomInfoPacket(buf, PACKET_ID::PT_CREATE_ROOM, created_room_idx, room_name, 0);
				ASyncWrite(buf);
			}
		}
	}
}

void Session::ProcessEnterRoom(const int room_index)
{
	if (m_cRoom.lock() != nullptr)
	{
		if (m_cRoom.lock()->GetRoomIndex() == 0)
		{
			if (EnterRoom(room_index))
			{
				// Notice Enter
				std::string msg = std::format("[ {} Entered Room {} ]", m_SessionID, room_index);
				ProcessChat(msg);

				// Send Result
				Buffer buf;
				NSW_PacketHandler::GetInstance().PackRoomInfoPacket(buf, PACKET_ID::PT_ENTER_ROOM, m_cRoom.lock()->GetRoomIndex(), m_cRoom.lock()->GetRoomName(), m_cRoom.lock()->GetParticipantCount());
				m_cRoom.lock()->Broadcast(buf);

				// Broadcast Room Info (Player List)
				ProcessUpdateRoom(room_index);

				// Broadcast Lobby Info (Room List)
				ProcessUpdateLobby();
			}
		}
	}
}

void Session::ProcessLeaveRoom()
{
	if (m_cRoom.lock() != nullptr)
	{
		if (!m_cRoom.lock()->IsPlayingGame())
		{
			uint8_t before_room_index = m_cRoom.lock()->GetRoomIndex();

			if (LeaveRoom())
			{
				// Send Result
				Buffer buf;
				NSW_PacketHandler::GetInstance().PackRequestPacket(buf, PACKET_ID::PT_LEAVE_ROOM);
				ASyncWrite(buf);

				// Broadcast Room Info (Player List)
				ProcessUpdateRoom(before_room_index);

				// Broadcast Lobby Info (Room List)
				ProcessUpdateLobby();
			}
		}
	}
}

void Session::ProcessUpdatePlayerState(const std::string player_id, const uint8_t player_state)
{
	if (m_cRoom.lock() != nullptr)
	{
		if (!m_cRoom.lock()->IsPlayingGame())
		{
			switch (player_state)
			{
			case PACKET_DATA::SessionPlayer::STATE_IN_ROOM::NONE:		ProcessLeaveRoom(); break;
			case PACKET_DATA::SessionPlayer::STATE_IN_ROOM::NOT_READY:	m_cRoom.lock()->RequestGameStart(shared_from_this(), PACKET_DATA::SessionPlayer::STATE_IN_ROOM::NOT_READY); break;
			case PACKET_DATA::SessionPlayer::STATE_IN_ROOM::READY:		m_cRoom.lock()->RequestGameStart(shared_from_this(), PACKET_DATA::SessionPlayer::STATE_IN_ROOM::READY); break;
			}
			
			Buffer SendBuffer;
			NSW_PacketHandler::GetInstance().PackPlayerInfoPacket(SendBuffer, PACKET_ID::PT_UPDATE_PLAYER_STATE, player_id, player_state);
			m_cRoom.lock()->Broadcast(SendBuffer);

			if (m_cRoom.lock()->IsPossibleGameStart())
			{
				m_cRoom.lock()->GameStart();

				ProcessGameStart();
			}
		}
	}
}

void Session::ProcessGameStart()
{
	if (m_cRoom.lock() != nullptr)
	{
		if (m_cRoom.lock()->IsPlayingGame())
		{
			std::vector<PACKET_DATA::SessionPlayer> PlayerList;
			m_cRoom.lock()->GetAllPlayer(PlayerList);

			int game_round = m_cChessGameManager.lock()->GetGameRound();
			std::string white_team_player = PlayerList.front().id;
			std::string black_team_player = PlayerList.back().id;

			Buffer SendBuffer;
			NSW_PacketHandler::GetInstance().PackGameInfoPacket(SendBuffer, PACKET_ID::PT_GAME_START, game_round, white_team_player, black_team_player);
			m_cRoom.lock()->Broadcast(SendBuffer);
		}
	}
}

void Session::ProcessChessEvent(const uint8_t chess_event, const std::string player_id)
{
	if (m_cRoom.lock() != nullptr)
	{
		if (m_cRoom.lock()->IsPlayingGame())
		{
			if (chess_event == PACKET_DATA::CHESS_EVENT::EVENT_TYPE::CHECKMATE || chess_event == PACKET_DATA::CHESS_EVENT::EVENT_TYPE::STALEMATE)
			{
				m_cChessGameManager.lock()->InitGame();
			}

			Buffer SendBuffer;
			NSW_PacketHandler::GetInstance().PackChessEventPacket(SendBuffer, PACKET_ID::PT_CHESS_EVENT, chess_event, player_id);
			m_cRoom.lock()->Broadcast(SendBuffer);
		}
	}
}

void Session::ProcessMovingChessPiece(const uint8_t round, const uint8_t src_slot_index, const uint8_t dest_slot_index)
{
	if (m_cRoom.lock() != nullptr)
	{
		if (m_cRoom.lock()->IsPlayingGame() && m_cChessGameManager.lock()->CheckSynchronize(round))
		{
			if (m_cChessGameManager.lock()->Move(src_slot_index, dest_slot_index))
			{
				uint8_t result = m_cChessGameManager.lock()->IsGameOver(round);
				if (result >= PACKET_DATA::CHESS_EVENT::EVENT_TYPE::NONE && result <= PACKET_DATA::CHESS_EVENT::EVENT_TYPE::STALEMATE)
				{
					int updated_round = m_cChessGameManager.lock()->GetGameRound();

					// Send Chess Move Event
					Buffer SendBuffer;
					NSW_PacketHandler::GetInstance().PackChessMovePacket(SendBuffer, PACKET_ID::PT_CHESS_MOVE, updated_round, src_slot_index, dest_slot_index);
					m_cRoom.lock()->Broadcast(SendBuffer);

					// Chess Event (None, Check, Checkmate, Stalemate)
					ProcessChessEvent(result, m_SessionID);
				}
			}
		}
		else
		{
			// 게임 중이 아닐 시 방 나가기 (방폭)
			ProcessLeaveRoom();
		}
	}
}

void Session::ProcessCastlingChessPiece(const uint8_t round, const uint8_t king_slot_index, const uint8_t rook_slot_index)
{
	if (m_cRoom.lock() != nullptr)
	{
		if (m_cRoom.lock()->IsPlayingGame() && m_cChessGameManager.lock()->CheckSynchronize(round))
		{
			if (m_cChessGameManager.lock()->Castling(king_slot_index, rook_slot_index))
			{
				uint8_t result = m_cChessGameManager.lock()->IsGameOver(round);
				if (result >= PACKET_DATA::CHESS_EVENT::EVENT_TYPE::NONE && result <= PACKET_DATA::CHESS_EVENT::EVENT_TYPE::STALEMATE)
				{
					int updated_round = m_cChessGameManager.lock()->GetGameRound();

					// Send Chess Castling Event
					Buffer SendBuffer;
					NSW_PacketHandler::GetInstance().PackChessCastlingPacket(SendBuffer, PACKET_ID::PT_CHESS_CASTLING, updated_round, king_slot_index, rook_slot_index);
					m_cRoom.lock()->Broadcast(SendBuffer);

					// Chess Event (Check, Check State Finish, Checkmate, Stalemate)
					ProcessChessEvent(result, m_SessionID);
				}
			}
		}
		else
		{
			// 게임 중이 아닐 시 방 나가기 (방폭)
			ProcessLeaveRoom();
		}
	}
}

void Session::ProcessPromoteChessPiece(const uint8_t round, const uint8_t pawn_slot_index, const uint8_t promotion_type)
{
	if (m_cRoom.lock() != nullptr)
	{
		if (m_cRoom.lock()->IsPlayingGame() && m_cChessGameManager.lock()->CheckSynchronize(round))
		{
			if (m_cChessGameManager.lock()->Promotion(pawn_slot_index, promotion_type))
			{
				uint8_t result = m_cChessGameManager.lock()->IsGameOver(round);
				if (result >= PACKET_DATA::CHESS_EVENT::EVENT_TYPE::NONE && result <= PACKET_DATA::CHESS_EVENT::EVENT_TYPE::STALEMATE)
				{
					int updated_round = m_cChessGameManager.lock()->GetGameRound();

					// Send Chess Promotion Event
					Buffer SendBuffer;
					NSW_PacketHandler::GetInstance().PackChessPromotionPacket(SendBuffer, PACKET_ID::PT_CHESS_PROMOTION, updated_round, pawn_slot_index, promotion_type);
					m_cRoom.lock()->Broadcast(SendBuffer);

					// Chess Event (Check, Check State Finish, Checkmate, Stalemate)
					ProcessChessEvent(result, m_SessionID);
				}
			}
		}
		else
		{
			// 게임 중이 아닐 시 방 나가기 (방폭)
			ProcessLeaveRoom();
		}
	}
}