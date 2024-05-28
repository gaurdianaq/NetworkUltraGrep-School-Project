/********************************************************************
 *Author: Evan Tatay-Hinds                                          *
 *Date: December 8th 2019                                           *
 *Description: Entry point for client                               *
 *Compiler: MSVC v142                                               *
 ********************************************************************/

#include <iostream>
#include "Client.hpp"
#include <thread>
#include <mutex>
#include <exception>
#include <ctime>

Client* client;

unsigned constexpr TIMEOUT = 10;

void communication()
{
	std::string command;
	std::mutex mtx;
	clock_t startTime, testTime, timePassed;
	double secondsPassed;
	while (client->getRunning())
	{
		startTime = clock();
		while (client->waitingForResponse()) //ensure we're not sending more stuff to the server while waiting for a response
		{
			testTime = clock();
			timePassed = testTime - startTime;
			secondsPassed = timePassed / (double)CLOCKS_PER_SEC;
			if (secondsPassed > TIMEOUT) //timeout if we don't receive a response from the server in time
			{
				std::cout << "Timed out waiting for a response from the server." << std::endl;
				client->notWaiting();
			}
		}
		std::cout << "\nEnter Command: \ngrep [-v] remotefolder regex.exts\nconnect ip port\ndrop\nstopserver\nexit\n\n";
		std::getline(std::cin, command);
		{
			std::unique_lock<std::mutex> lk(mtx);
			client->ProcessCommand(command);
		}
	}
}

int main(int argc, char* argv[])
{
	client = nullptr;
	if (argc == 1)
	{
		client = new Client();
	}
	if (argc == 2)
	{
		client = new Client(argv[1]);
	}
	else if (argc == 3)
	{
		unsigned short port;
		try
		{
			port = std::stoi(argv[2]);
			client = new Client(argv[1], port);
		}
		catch (std::exception ex)
		{
			std::cout << ex.what() << std::endl;
		}		
	}
	if (client != nullptr)
	{
		std::thread communicationThread{ communication };
		communicationThread.detach();
		client->Listen();
		delete client;
	}
	else
	{
		std::cout << "Could not start client!" << std::endl;
	}
}