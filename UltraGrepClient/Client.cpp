/********************************************************************
 *Author: Evan Tatay-Hinds                                          *
 *Date: December 8th 2019                                           *
 *Description: Client method definitions                            *
 *Compiler: MSVC v142                                               *
 ********************************************************************/

#include "Client.hpp"
#include <iostream>
#include <ws2tcpip.h>
#include <vector>
#include <regex>
#include <thread>
#include <mutex>
#include <filesystem>

std::mutex consolemtx;
unsigned short constexpr DEFAULT_PORT = 27015;

Client::~Client()
{

}

/*
Purpose: Instantiates client with default values
Accepts: nothing
Returns: nothing
*/
Client::Client()
{
	resultInfo = WSAStartup(MAKEWORD(2, 2), &wsaData);
	waiting = false;
	running = true;
	if (resultInfo != 0)
	{
		std::cerr << "WSAStartup failed: " << resultInfo << std::endl;
		exit(EXIT_FAILURE);
	}
	connectionSocket = INVALID_SOCKET;
	Connect("127.0.0.1");
}

/*
Purpose: Instantiates client with specified server address
Accepts: Server Address
Returns: nothing
*/
Client::Client(const char* serverAddr)
{
	resultInfo = WSAStartup(MAKEWORD(2, 2), &wsaData);
	waiting = false;
	running = true;
	if (resultInfo != 0)
	{
		std::cerr << "WSAStartup failed: " << resultInfo << std::endl;
		exit(EXIT_FAILURE);
	}
	connectionSocket = INVALID_SOCKET;
	Connect(serverAddr);
}

/*
Purpose: Instantiates client with specified server address and server port
Accepts: Server Address & Server Port
Returns: nothing
*/
Client::Client(const char* serverAddr, const unsigned short serverPort)
{
	resultInfo = WSAStartup(MAKEWORD(2, 2), &wsaData);
	waiting = false;
	running = true;
	if (resultInfo != 0)
	{
		std::cerr << "WSAStartup failed: " << resultInfo << std::endl;
		exit(EXIT_FAILURE);
	}
	connectionSocket = INVALID_SOCKET;
	Connect(serverAddr, serverPort);
}

/*
Purpose: Connects client to server at specified address
Accepts: Server Address
Returns: true if successful and false on failure
*/
bool Client::Connect(const char* serverAddr)
{
	if (connectionSocket == INVALID_SOCKET)
	{
		connectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		serverInfo = { 0 };
		serverInfo.sin_family = AF_INET;
		serverInfo.sin_port = htons(DEFAULT_PORT);
		ZeroMemory(buffer, DEFAULT_BUFLEN);
		inet_pton(AF_INET, serverAddr, &(serverInfo.sin_addr));
		if (connect(connectionSocket, (SOCKADDR*)& serverInfo, sizeof(serverInfo)) == SOCKET_ERROR) {
			std::cerr << "connect() failed" << std::endl;
			connectionSocket = INVALID_SOCKET;
			return false;
		}
		std::cout << "Connected to server at " << serverAddr << std::endl;
		return true;
	}
	else
	{
		std::cout << "Already connected to server!\n";
		return false;
	}
}

/*
Purpose: Connects client to server at specified address at specified port
Accepts: Server Address & Server Port
Returns: true if successful and false on failure
*/
bool Client::Connect(const char* serverAddr, const unsigned short serverPort)
{
	if (connectionSocket == INVALID_SOCKET)
	{
		connectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		serverInfo = { 0 };
		serverInfo.sin_family = AF_INET;
		serverInfo.sin_port = htons(serverPort);
		ZeroMemory(buffer, DEFAULT_BUFLEN);
		inet_pton(AF_INET, serverAddr, &(serverInfo.sin_addr));
		if (connect(connectionSocket, (SOCKADDR*)& serverInfo, sizeof(serverInfo)) == SOCKET_ERROR) {
			std::cerr << "connect() failed" << std::endl;
			connectionSocket = INVALID_SOCKET;
			return false;
		}
		std::cout << "Connected to server at " << serverAddr << " on port " << serverPort << std::endl;
		return true;
	}
	else
	{
		std::cout << "Already connected to server!\n";
		return false;
	}
}

/*
Purpose: Sends a grep command with the required info to the server
Accepts: Verbose Mode, file path, regex, extensions
Returns: true if successful and false on failure
*/
bool Client::SendGrep(bool verbose, const char* path, std::string regex, std::string extensions)
{
	if (connectionSocket != INVALID_SOCKET)
	{
		std::string message;
		if (verbose)
		{
			message += "1";
			message += path;
			message += ' ' + regex + ' ' + extensions;
			strcpy_s(buffer, message.c_str());
			send(connectionSocket, buffer, DEFAULT_BUFLEN, 0);
		}
		else
		{
			message += "0";
			message += path;
			message += ' ' + regex + ' ' + extensions;
			strcpy_s(buffer, message.c_str());
			send(connectionSocket, buffer, DEFAULT_BUFLEN, 0);
		}
		waiting = true;
	}
	else
	{
		std::cout << "Not connected to a server!\n";
		return false;
	}
}

/*
Purpose: Processes commands inputted by the user
Accepts: Command
Returns: nothing
*/
void Client::ProcessCommand(std::string command)
{
	const char* VERBOSE_FLAG = "-v";
	std::vector<std::string> parameters;
	std::string value = "";
	for (unsigned i = 0; i < command.length(); ++i)
	{
		if ((command[i] == ' ' && i > 0) || i == command.length()-1)
		{
			if (i == command.length() - 1 && command[i] != ' ')
			{
				value += command[i];
			}

			parameters.push_back(value);
			value = "";
		}
		if (command[i] != ' ')
		{
			value += command[i];
		}
	}
	if (parameters.size() == 0)
	{
		std::cout << "You didn't enter anything...\n";
	}
	else
	{
		if (parameters[0] == "grep")
		{
			if (parameters.size() < 3)
			{
				std::cout << "Not enough parameters" << std::endl;
			}
			else
			{
				std::experimental::filesystem::path folder;
				std::regex expr;
				std::string exprstr;
				std::string fileExtensions = ".txt";
				bool verbose = false;
				if (parameters[1] == VERBOSE_FLAG && parameters.size() > 3)
				{
					folder = parameters[2];
					expr = parameters[3];
					exprstr = parameters[3];
					verbose = true;
					if (parameters.size() > 4)
					{
						fileExtensions = parameters[4];
					}
				}
				else
				{
					folder = parameters[1];
					expr = parameters[2];
					exprstr = parameters[2];
					if (parameters.size() > 3)
					{
						fileExtensions = parameters[3];
					}
				}
				if (!expr._Empty())
				{
					SendGrep(verbose, folder.generic_u8string().c_str(), exprstr, fileExtensions);
				}
				else
				{
					std::cout << "No expression provided or expression is not valid!" << std::endl;
				}
			}
		}
		else if (parameters[0] == "connect")
		{
			if (connectionSocket == INVALID_SOCKET)
			{
				if (parameters.size() == 1)
				{
					Connect("127.0.0.1");
				}
				else if (parameters.size() == 2)
				{
					Connect(parameters[1].c_str());
				}
				else if (parameters.size() > 2) //ignore any extra parameters provided beyond the port
				{
					unsigned short port = std::stoi(parameters[2]);
					if (port != 0)
					{
						Connect(parameters[1].c_str(), port);
					}
					else
					{
						std::cout << "Port not valid, needs to be a number!" << std::endl;
					}
				}
			}
			else
			{
				std::cout << "Already connected! Drop the connection first then try to connect!" << std::endl;
			}
			
		}
		else if (parameters[0] == "drop")
		{
			if (connectionSocket == INVALID_SOCKET)
			{
				std::cout << "Not currently connected! Nothing to drop!" << std::endl;
			}
			else
			{
				ZeroMemory(buffer, DEFAULT_BUFLEN);
				buffer[0] = '2';
				send(connectionSocket, buffer, DEFAULT_BUFLEN, 0);
			}
		}
		else if (parameters[0] == "stopserver")
		{
			if (connectionSocket == INVALID_SOCKET)
			{
				std::cout << "Can't shutdown the server if you're not connected!" << std::endl;
			}
			else
			{
				ZeroMemory(buffer, DEFAULT_BUFLEN);
				buffer[0] = '3';
				send(connectionSocket, buffer, DEFAULT_BUFLEN, 0);
			}
		}
		else if (parameters[0] == "exit")
		{
			if (connectionSocket != INVALID_SOCKET)
			{
				ZeroMemory(buffer, DEFAULT_BUFLEN);
				buffer[0] = '2';
				send(connectionSocket, buffer, DEFAULT_BUFLEN, 0);
			}
			running = false;
		}
		else
		{
			std::cout << "Invalid command, commands are grep, connect, drop, stopserver, and exit\n";
		}
	}
	
}

/*
Purpose: Listens for data from the server
Accepts: nothing
Returns: nothing
*/
void Client::Listen()
{
	while (running)
	{
		while (connectionSocket != INVALID_SOCKET)
		{
			std::unique_lock<std::mutex> lk(consolemtx);
			ZeroMemory(inBuffer, DEFAULT_BUFLEN);
			recv(connectionSocket, inBuffer, DEFAULT_BUFLEN, 0);
			std::string data = inBuffer;
			if (data == "0")
			{
				waiting = false;
			}
			else if (data == "1")
			{
				waiting = false;
				std::cout << "No such filepath exists on the server" << std::endl;
			}
			else if (data == "2") //bit of a handshake with the server to ensure the socket is cleanly closed
			{
				closesocket(connectionSocket);
				connectionSocket = INVALID_SOCKET;
			}
			else
			{
				std::cout << inBuffer << std::endl;
			}
		}
	}
	WSACleanup();
}

/*
Purpose: getter for the waiting variable
Accepts: nothing
Returns: waiting
*/
bool Client::waitingForResponse() const
{
	return waiting;
}

/*
Purpose: getter for the running variable
Accepts: nothing
Returns: running
*/
bool Client::getRunning() const
{
	return running;
}

/*
Purpose: sets waiting to false
Accepts: nothing
Returns: nothing
*/
void Client::notWaiting()
{
	waiting = false;
}