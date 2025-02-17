#pragma once
#include <iostream>
#include <map>

#include "ChessGamePacket.hpp"

class Room
{
public:
	Room(const int room_id, const std::string name);

	~Room()
	{
		m_PlayerMap.clear();
	}

public:
	bool IsPossibleGameStart() const;
	void RequestGameStart(std::shared_ptr<class Session> pSession, uint8_t state);
	void GameStart();

public:
	void Enter(std::shared_ptr<class Session> pSession);
	void Leave(std::shared_ptr<class Session> pSession);
	void Broadcast(class Buffer& buf);

public:
	void GetAllPlayer(std::vector<PACKET_DATA::SessionPlayer>& PlayerList);

public:
	bool IsPlayingGame() const;
	bool IsFull() const { return (m_PlayerMap.size() >= 2); }
	bool IsEmpty() const { return (m_PlayerMap.size() <= 0); }

public:
	int GetRoomIndex() const { return m_iRoomID; }
	int GetParticipantCount() const { return m_PlayerMap.size(); }
	std::string GetRoomName() const { return m_strRoomName; }

private:
	void InitRoomState();

private:
	int m_iRoomID;
	std::string m_strRoomName;

private:
	struct SESSION_STATE
	{
		uint8_t state;
		uint8_t round = 0;
	};

	std::map<std::shared_ptr<class Session>, SESSION_STATE> m_PlayerMap;

private:
	std::shared_ptr<class ChessGameManager> m_cChessGameManager;
};