/********************************************************************
 *Author: Evan Tatay-Hinds                                          *
 *Date: December 8th 2019                                           *
 *Description: Contains method declarations for sending data to     *
 *			   the output queue         							*
 *Compiler: MSVC v142                                               *
 ********************************************************************/

#ifndef _LOCKED_FUNCTIONS
#define _LOCKED_FUNCTIONS

#include <string>
#include "ThreadSafe_Queue.hpp"

void setOutputQueue(threadsafe_queue<std::string>* outputQueue);
void deleteOutputQueue();
void SendMessage(std::string message);

#endif