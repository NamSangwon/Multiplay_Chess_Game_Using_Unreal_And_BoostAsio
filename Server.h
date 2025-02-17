#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <unordered_set>
#include <map>

#include "ChessGamePacket.hpp"

const unsigned short PORT_NUMBER = 31400;

class TCP_Server : public std::enable_shared_from_this<TCP_Server>
{
public:
	TCP_Server(boost::asio::io_service& io_service) :
		m_io_service(io_service),
		m_Acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT_NUMBER))
	{
		CreateRoom("Lobby");
	}

	~TCP_Server()
	{
		Stop();
	}

public:
	void Start();
	void Stop();
	void CloseSession(std::shared_ptr<class Session> pSession);

private:
	void StartAccept();
	void HandleAccept(std::shared_ptr<class Session> pSession, const boost::system::error_code& error);

private:
	boost::asio::io_service& m_io_service;
	boost::asio::ip::tcp::acceptor m_Acceptor;

	boost::thread_group ThreadGroup;
	const int ThreadNum = 5;

public:
	int CreateRoom(const std::string& room_name);
	std::shared_ptr<class Room> GetRoom(int room_idx);
	void GetAllRoom(std::vector<PACKET_DATA::SessionRoom>& RoomList);
	void DeleteRooms();

public:
	bool IsDuplicatedID(const std::string id);

private:
	std::unordered_set<std::shared_ptr<class Session>> m_vSessions;
	std::map<int, std::shared_ptr<class Room>> m_RoomMap;

	int m_nRoomCount = 0;
};