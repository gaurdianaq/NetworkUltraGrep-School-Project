/********************************************************************
 *Author: Evan Tatay-Hinds                                          *
 *Date: December 8th 2019                                           *
 *Description: Server class declaration                             *
 *Compiler: MSVC v142                                               *
 ********************************************************************/

#ifndef _SERVER
#define _SERVER

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>

#include "UltraGrep.hpp"
#include "ThreadSafe_Queue.hpp"

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 256

class Server
{
	WSAData wsaData;
	unsigned short const listenPort;
	SOCKET listenSocket;
	SOCKET clientSocket;
	sockaddr_in serverInfo;
	UltraGrep* ultraGrep;
	threadsafe_queue<std::string> outputQueue;
	char buffer[DEFAULT_BUFLEN];
	int resultInfo;
	bool invalidFilePath;
	bool shutDown;
	bool Start();
	void Poll();
	std::thread grepThread;
public:
	Server();
	Server(const char* serverAddr);
	Server(const char* serverAddr, const unsigned short serverPort);
	~Server();
	void Launch();
};

#endif