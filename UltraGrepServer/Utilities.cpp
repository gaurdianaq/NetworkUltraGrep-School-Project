/********************************************************************
 *Author: Evan Tatay-Hinds                                          *
 *Date: November 8th 2019                                           *
 *Description: Contains method definitions for outputting to the    *
 *			   output queue, as wwell as the actual outputqueue		*
 *Compiler: MSVC v142                                               *
 ********************************************************************/

#include "Utilities.hpp"
#include <iostream>

threadsafe_queue<std::string>* _outputQueue = nullptr;

void setOutputQueue(threadsafe_queue<std::string>* outputQueue)
{
	if (_outputQueue == nullptr) //ensures it doesn't get overwritten without intentionally clearing it
	{
		_outputQueue = outputQueue;
	}
}

void deleteOutputQueue()
{
	_outputQueue = nullptr;
}

void SendMessage(std::string message)
{
	_outputQueue->push(new std::string(message));
}