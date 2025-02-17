#include "Server.h"

#include "Session.h"
#include "Room.h"

void TCP_Server::Start()
{
	m_io_service.post(boost::bind(&TCP_Server::StartAccept, this));

	for (int i = 0; i < ThreadNum; i++)
	{
		ThreadGroup.create_thread(
			boost::bind(&boost::asio::io_service::run, &m_io_service)
		);
	}

	ThreadGroup.join_all();
}

void TCP_Server::Stop()
{
	for (auto pSession : m_vSessions)
	{
		pSession->Disconnect();
	}
	m_vSessions.clear();

	for (auto pRoom : m_RoomMap)
	{
		pRoom.second.reset();
	}
	m_RoomMap.clear();

	m_io_service.stop();
	m_io_service.reset();
}

void TCP_Server::StartAccept()
{
	std::cout << "[" << boost::this_thread::get_id() << "] " << "Wait Client Accepting....." << std::endl;

	std::shared_ptr<Session> pSession = std::make_shared<Session>(m_io_service, shared_from_this());

	m_Acceptor.async_accept(
		pSession->GetSocket(),
		boost::bind(
				&TCP_Server::HandleAccept, 
				this,
				pSession,
				boost::asio::placeholders::error
		)
	);
}

void TCP_Server::HandleAccept(std::shared_ptr<Session> pSession, const boost::system::error_code& error)
{
	if (pSession != nullptr)
	{
		if (error)
		{
			std::cout << error.what() << std::endl;

			pSession->Disconnect();
			pSession.reset();
		}
		else
		{
			m_vSessions.insert(pSession);

			std::cout << "[" << boost::this_thread::get_id() << "] " << "Client Accepted" << std::endl;

			m_io_service.post(boost::bind(&Session::ASyncRead, pSession));
		}

		StartAccept();
	}
}

void TCP_Server::CloseSession(std::shared_ptr<class Session> pSession)
{
	if (pSession)
	{
		m_vSessions.erase(pSession);
	}
}

int TCP_Server::CreateRoom(const std::string& room_name)
{
	int created_room_idx = -1;
	std::shared_ptr<Room> created_room = std::make_shared<Room>(m_nRoomCount, room_name);
	if (created_room)
	{
		created_room_idx = m_nRoomCount;

		m_RoomMap[created_room_idx] = created_room;

		m_nRoomCount++;
	}

	return created_room_idx;
}

std::shared_ptr<class Room> TCP_Server::GetRoom(int room_idx)
{
	return m_RoomMap[room_idx];
}

void TCP_Server::GetAllRoom(std::vector<PACKET_DATA::SessionRoom>& RoomList)
{
	// Exclude Lobby (i = 0)
	for (int i = 1; i < m_RoomMap.size(); i++)
	{
		if (m_RoomMap[i] != nullptr)
		{
			PACKET_DATA::SessionRoom session_room;
			session_room.index = i;

			std::string room_name = m_RoomMap[i]->GetRoomName();
			memcpy(session_room.name, room_name.c_str(), sizeof(session_room.name));

			session_room.participant_num = m_RoomMap[i]->GetParticipantCount();

			RoomList.push_back(session_room);
		}
	}
}

void TCP_Server::DeleteRooms()
{
	// Exclude Lobby (i = 0)
	for (int i = 1; i < m_RoomMap.size(); i++)
	{
		if (m_RoomMap[i] != nullptr && m_RoomMap[i]->IsEmpty())
		{
			m_RoomMap[i].reset();
			m_RoomMap.erase(i);
		}
	}
}

bool TCP_Server::IsDuplicatedID(const std::string id)
{
	for (auto& pSession : m_vSessions)
	{
		std::string existed_id = pSession->GetSessionID();

		if (id == existed_id)
		{
			return true;
		}
	}

	return false;
}
