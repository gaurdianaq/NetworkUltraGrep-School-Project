/********************************************************************
 *Author: Evan Tatay-Hinds                                          *
 *Date: December 8th 2019                                           *
 *Description: Entry point for server                               *
 *Compiler: MSVC v142                                               *
 ********************************************************************/

#include "Server.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
	Server* server = nullptr;
	if (argc == 1)
	{
		server = new Server();
	}
	else if (argc == 2)
	{
		server = new Server(argv[1]);
	}
	else if (argc == 3)
	{
		short port = std::stoi(argv[2]);
		server = new Server(argv[1], port);
	}
	if (server != nullptr)
	{
		server->Launch();
	}
	else
	{
		std::cout << "Server did not initialize properly...\n";
	}
	if (server != nullptr)
	{
		delete server;
	}
}