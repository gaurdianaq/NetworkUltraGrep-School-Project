/********************************************************************
 *Author: Evan Tatay-Hinds                                          *
 *Date: December 8th 2019                                           *
 *Description: Client class declaration                             *
 *Compiler: MSVC v142                                               *
 ********************************************************************/

#ifndef _CLIENT
#define _CLIENT

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <string>
#include <atomic>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 256

class Client
{
	WSAData wsaData;
	char buffer[DEFAULT_BUFLEN];
	char inBuffer[DEFAULT_BUFLEN];
	int resultInfo;
	SOCKET connectionSocket;
	sockaddr_in serverInfo;
	bool Connect(const char* serverAddr);
	bool Connect(const char* serverAddr, const unsigned short serverPort);
	std::atomic<bool> waiting;
	std::atomic<bool> running;
	bool SendGrep(bool verbose, const char* path, std::string regex, std::string extensions);
public:
	Client();
	Client(const char* serverAddr);
	Client(const char* serverAddr, const unsigned short serverPort);
	~Client();
	void Listen();
	bool getRunning() const;
	bool waitingForResponse() const;
	void notWaiting();
	void ProcessCommand(std::string command);
};

#endif