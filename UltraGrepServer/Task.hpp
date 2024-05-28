/********************************************************************
 *Author: Evan Tatay-Hinds                                          *
 *Date: November 8th 2019                                           *
 *Description: Contains class declaration for Task                  *
 *Compiler: MSVC v142                                               *
 ********************************************************************/

#ifndef _TASK
#define _TASK

//Abstract interface meant to represent some sort of task, could be anything really...
class Task
{
public:
	virtual void run() = 0;
};

#endif