#include <iostream>
#include <boost/asio.hpp>

#include "Server.h"

using namespace std;

int main()
{
	try
	{
		boost::asio::io_service io_service;
		std::shared_ptr<TCP_Server> server = std::make_shared<TCP_Server>(io_service);

		server->Start();
	}
	catch (exception& e)
	{
		cout << e.what(); '\n';
	}

	std::cout << "네트워크 접속 종료" << std::endl;

	return 0;
}