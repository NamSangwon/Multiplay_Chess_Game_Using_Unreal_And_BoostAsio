#include "Room.h"

#include "Session.h"
#include "Buffer.hpp"
#include "ChessGameManager.h"

Room::Room(const int room_id, const std::string name)
{
	m_iRoomID = room_id;
	m_strRoomName = name;
	m_cChessGameManager = std::make_shared<ChessGameManager>();
}

void Room::RequestGameStart(std::shared_ptr<class Session> pSession, uint8_t state)
{
	m_PlayerMap[pSession].state = state;
}

void Room::GameStart()
{
	InitRoomState();

	if (m_cChessGameManager)
	{
		m_cChessGameManager->GameStart();
	}
}

void Room::Enter(std::shared_ptr<Session> pSession)
{
	if (!IsFull())
	{
		SESSION_STATE player_state = { PACKET_DATA::SessionPlayer::STATE_IN_ROOM::NOT_READY, 0 };
		m_PlayerMap.insert(std::make_pair(pSession, player_state));
		pSession->SetChessGameManager(this->m_cChessGameManager);
	}
}

void Room::Leave(std::shared_ptr<Session> pSession)
{
	m_PlayerMap.erase(pSession);

	InitRoomState();
}

void Room::Broadcast(Buffer& buf)
{
	for (auto& pSession : m_PlayerMap)
	{
		// Broadcast
		pSession.first->ASyncWrite(buf);
	}
}

void Room::GetAllPlayer(std::vector<PACKET_DATA::SessionPlayer>& PlayerList)
{
	for (auto& pSession : m_PlayerMap)
	{
		PACKET_DATA::SessionPlayer player;

		std::string session_id = pSession.first->GetSessionID();
		if (!session_id.empty())
		{
			memcpy(player.id, session_id.c_str(), sizeof(player.id));
			player.state = pSession.second.state;

			PlayerList.push_back(player);
		}
	}
}

bool Room::IsPlayingGame() const
{
	if (m_cChessGameManager)
	{
		return (m_cChessGameManager->GetGameRound() > 0);
	}

	return false;
}

bool Room::IsPossibleGameStart() const
{
	if (IsFull())
	{
		for (auto& player : m_PlayerMap)
		{
			if (player.second.state != PACKET_DATA::SessionPlayer::STATE_IN_ROOM::READY)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

void Room::InitRoomState()
{
	if (m_cChessGameManager)
	{
		m_cChessGameManager->InitGame();
	}

	for (auto& player : m_PlayerMap)
	{
		player.second.state = PACKET_DATA::SessionPlayer::STATE_IN_ROOM::NOT_READY;
		player.second.round = 0;
	}
}