#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "Buffer.hpp"

class Session : public std::enable_shared_from_this<Session>
{
public:
	Session(boost::asio::io_context& io_context, std::shared_ptr<class TCP_Server> Owner) :
		m_Socket(io_context),
		m_Strand(boost::asio::make_strand(io_context)),
		m_cServer(Owner) 
	{
		m_bDisconnected.exchange(false);
		memset(m_cBuffer, '\0', sizeof(m_cBuffer));
	}

	~Session() 
	{ 
		std::cout << "{" << this << "} Session is Destoryed" << std::endl;
	}

public:
	void ASyncRead();
	void ASyncWrite(class Buffer& buf);
	void Disconnect();

private:
	void HandleRead(const boost::system::error_code& error, size_t bytes_transferred);
	void HandleWrite(const boost::system::error_code& error, size_t bytes_transferred);

private:
	bool ResponsePacket();
	bool ResponseProgramStart(class Buffer& RecvPacket, uint16_t& content_length);
	bool ResponseChat(class Buffer& RecvPacket, uint16_t& content_length);
	bool ResponseUpdateLobby(class Buffer& RecvPacket, uint16_t& content_length);
	bool ResponseUpdateRoom(class Buffer& RecvPacket, uint16_t& content_length);
	bool ResponseCreateRoom(class Buffer& RecvPacket, uint16_t& content_length);
	bool ResponseEnterRoom(class Buffer& RecvPacket, uint16_t& content_length);
	bool ResponseEnterLobby(class Buffer& RecvPacket, uint16_t& content_length);
	bool ResponseUpdatePlayerState(class Buffer& RecvPacket, uint16_t& content_length);
	bool ResponseMovingChessPiece(class Buffer& RecvPacket, uint16_t& content_length);
	bool ResponseCastlingChessPiece(class Buffer& RecvPacket, uint16_t& content_length);
	bool ResponsePromoteChessPiece(class Buffer& RecvPacket, uint16_t& content_length);

private:
	void ProcessProgramStart(const std::string id);
	void ProcessChat(const std::string chat_msg);
	void ProcessCreateRoom(const std::string room_name);
	void ProcessEnterRoom(const int room_index);
	void ProcessLeaveRoom();
	void ProcessUpdatePlayerState(const std::string player_id, const uint8_t player_state);
	void ProcessGameStart();
	void ProcessChessEvent(const uint8_t chess_event, const std::string player_id);
	void ProcessMovingChessPiece(const uint8_t round, const uint8_t src_slot_index, const uint8_t dest_slot_index);
	void ProcessCastlingChessPiece(const uint8_t round, const uint8_t king_slot_index, const uint8_t rook_slot_index);
	void ProcessPromoteChessPiece(const uint8_t round, const uint8_t pawn_slot_index, const uint8_t promotion_type);

public:
	void ProcessUpdateLobby();
	void ProcessUpdateRoom(const uint8_t room_index);

private:
	bool EnterRoom(const uint8_t& room_index);
	bool LeaveRoom();

public:
	void SetChessGameManager(std::shared_ptr<class ChessGameManager>& chess_game_manager) { m_cChessGameManager = chess_game_manager; }

public:
	void SetSessionID(const std::string SessionID) { m_SessionID = SessionID.c_str(); }
	std::string GetSessionID() { return m_SessionID; }

public:
	boost::asio::ip::tcp::socket& GetSocket() { return m_Socket; }

private:
	std::atomic<bool> m_bDisconnected;
	boost::asio::ip::tcp::socket m_Socket;
	boost::asio::strand<boost::asio::io_context::executor_type> m_Strand;

	std::weak_ptr<class TCP_Server> m_cServer;
	char m_cBuffer[PACKET_SIZE];

private:
	std::weak_ptr<class Room> m_cRoom;
	std::weak_ptr<class ChessGameManager> m_cChessGameManager;
	std::string m_SessionID;
};