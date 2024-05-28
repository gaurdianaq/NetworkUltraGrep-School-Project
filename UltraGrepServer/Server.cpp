/********************************************************************
 *Author: Evan Tatay-Hinds                                          *
 *Date: December 8th 2019                                           *
 *Description: Server method definitions                            *
 *Compiler: MSVC v142                                               *
 ********************************************************************/

#include "Server.hpp"

#include <iostream>
#include <thread>
#include <ws2tcpip.h>
#include <regex>
#include "Utilities.hpp"
#include <filesystem>

unsigned short constexpr DEFAULT_PORT = 27015;
char constexpr OK = '0';
char constexpr INVALID_GREP = '1';
char constexpr DROP = '2';

Server::~Server()
{

}

/*
Purpose: Instantiates server instance with default values
Accepts: nothing
Returns: nothing
*/
Server::Server() : listenPort(DEFAULT_PORT)
{
	resultInfo = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (resultInfo != 0)
	{
		std::cerr << "WSAStartup failed: " << resultInfo << std::endl;
		exit(EXIT_FAILURE);
	}
	serverInfo = { 0 };
	listenSocket = INVALID_SOCKET;
	clientSocket = INVALID_SOCKET;
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = htons(listenPort);
	ZeroMemory(buffer, DEFAULT_BUFLEN);
	inet_pton(AF_INET, "127.0.0.1", &(serverInfo.sin_addr));
}

/*
Purpose: Instantiates server instance with specified IP address
Accepts: Server Address
Returns: nothing
*/
Server::Server(const char* serverAddr) : listenPort(DEFAULT_PORT)
{
	resultInfo = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (resultInfo != 0)
	{
		std::cerr << "WSAStartup failed: " << resultInfo << std::endl;
		exit(EXIT_FAILURE);
	}
	serverInfo = { 0 };
	listenSocket = INVALID_SOCKET;
	clientSocket = INVALID_SOCKET;
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = htons(listenPort);
	ZeroMemory(buffer, DEFAULT_BUFLEN);
	inet_pton(AF_INET, serverAddr, &(serverInfo.sin_addr));
}

/*
Purpose: Instantiates server instance with specified IP address and server port
Accepts: Server Address & Server Port
Returns: nothing
*/
Server::Server(const char* serverAddr, const unsigned short serverPort): listenPort(serverPort)
{
	resultInfo = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (resultInfo != 0)
	{
		std::cerr << "WSAStartup failed: " << resultInfo << std::endl;
		exit(EXIT_FAILURE);
	}
	serverInfo = { 0 };
	listenSocket = INVALID_SOCKET;
	clientSocket = INVALID_SOCKET;
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = htons(listenPort);
	ZeroMemory(buffer, DEFAULT_BUFLEN);
	inet_pton(AF_INET, serverAddr, &(serverInfo.sin_addr));
}

/*
Purpose: Sets up the listen socket
Accepts: nothing
Returns: true if server started successfully and false if it didn't
*/
bool Server::Start()
{
	if (resultInfo != 0)
	{
		std::cout << "WSAStartup failed with error: " << resultInfo << std::endl;
		return false;
	}

	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
	{
		std::cout << "socket failed with error: " << WSAGetLastError() << std::endl;
		return false;
	}

	resultInfo = bind(listenSocket, (SOCKADDR*)&serverInfo, sizeof(serverInfo));
	if (resultInfo == SOCKET_ERROR)
	{
		std::cout << "bind failed with error: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		return false;
	}

	resultInfo = listen(listenSocket, SOMAXCONN);
	if (resultInfo == SOCKET_ERROR)
	{
		std::cout << "listen failed with error: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		return false;
	}

	setOutputQueue(&outputQueue);

	return true;
}

/*
Purpose: Listens for messages from the connected client, if no client is connected it idles and waits for a new client to connect
Accepts: nothing
Returns: nothing
*/
void Server::Poll()
{
	while (!shutDown)
	{
		std::cout << "Waiting for connection\n";
		while (clientSocket == INVALID_SOCKET) {
			clientSocket = accept(listenSocket, NULL, NULL);
		}
		for (;;)
		{
			std::cout << "Listening for client command..." << std::endl;
			ZeroMemory(buffer, DEFAULT_BUFLEN);
			int bytesreceived = recv(clientSocket, buffer, DEFAULT_BUFLEN, 0);
			std::string command = buffer;
			std::string params[3];
			command = command.substr(1, command.length());
			unsigned n = 0;
			for (unsigned i = 0; i < command.length() + 1; ++i) //parses our single extensions string and puts each extension into a vector of strings
			{
				if ((command[i] == ' ' && i > 0) || i == command.length() - 1)
				{
					if (i == command.length() - 1 && command[i] != ' ')
					{
						params[n] += command[i];
					}
					if (i != command.length() - 1)
					{
						++n;
					}
				}
				if (command[i] != ' ' && i != command.length() - 1)
				{
					params[n] += command[i];
				}

			}
			if (buffer[0] == '0')
			{
				std::experimental::filesystem::path filePath = params[0];
				if (std::experimental::filesystem::is_directory(filePath))
				{
					invalidFilePath = false;
					ultraGrep = new UltraGrep();
					std::thread grepThread{ &UltraGrep::startGrep, ultraGrep, params[0], std::regex(params[1]), false, params[2] };
					grepThread.detach();
				}
				else
				{
					invalidFilePath = true;
				}
				
				
			}
			else if (buffer[0] == '1')
			{
				std::experimental::filesystem::path filePath = params[0];
				if (std::experimental::filesystem::is_directory(filePath))
				{
					invalidFilePath = false;
					ultraGrep = new UltraGrep();
					std::thread grepThread{ &UltraGrep::startGrep, ultraGrep, params[0], std::regex(params[1]), true, params[2] };
					grepThread.detach();
				}
				else
				{
					invalidFilePath = true;
				}
			}
			else if (buffer[0] == '2')
			{
				//drop code will go here
				ZeroMemory(buffer, DEFAULT_BUFLEN);
				buffer[0] = DROP;
				send(clientSocket, buffer, DEFAULT_BUFLEN, 0);
				resultInfo = closesocket(clientSocket);
				clientSocket = INVALID_SOCKET;
				std::cout << "Client disconnected" << std::endl;
				break;
			}
			else if (buffer[0] == '3')
			{
				ZeroMemory(buffer, DEFAULT_BUFLEN);
				buffer[0] = DROP;
				send(clientSocket, buffer, DEFAULT_BUFLEN, 0);
				closesocket(clientSocket);
				clientSocket = INVALID_SOCKET;
				std::cout << "Client disconnected" << std::endl;
				shutDown = true;
				std::cout << "Shutting down server" << std::endl;
			}

			if (!invalidFilePath)
			{
				while (!ultraGrep->isDone() || !outputQueue.empty()) //so long as ultragrep is not done or the output queue still has stuff in it, we continue to send data to the client
				{
					if (ultraGrep->isDone())
					{
						ZeroMemory(buffer, DEFAULT_BUFLEN);
						std::string* message = outputQueue.try_pop();
						if (message != nullptr)
						{
							strcpy_s(buffer, message->c_str());
							send(clientSocket, buffer, DEFAULT_BUFLEN, 0);
						}
					}
				}
			}
			
			if (ultraGrep != nullptr) //shouldn't get to to this point till ultragrep is done it's task
			{
				delete ultraGrep;
			}
			if (clientSocket != INVALID_SOCKET)
			{
				ZeroMemory(buffer, DEFAULT_BUFLEN);
				if (!invalidFilePath)
				{
					buffer[0] = OK; //0 will be interpretted by the client meaning success
					send(clientSocket, buffer, 1, 0);
				}
				else
				{
					buffer[0] = INVALID_GREP; //1 will be interpretted by the client meaning that the provided file path was invalid
					send(clientSocket, buffer, 1, 0);
				}
				
			}
		}
	}
	closesocket(listenSocket);
	listenSocket = INVALID_SOCKET;
	WSACleanup();
}

/*
Purpose: Calls start, if it succeeds it launches the poll method
Accepts: nothing
Returns: nothing
*/
void Server::Launch()
{
	if (Start())
	{
		Poll();
	}
}