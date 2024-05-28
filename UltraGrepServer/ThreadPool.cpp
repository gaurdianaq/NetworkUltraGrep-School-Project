/********************************************************************
 *Author: Anthony Williams & Evan Tatay-Hinds                       *
 *Date: November 8th 2019                                           *
 *Description: Contains method definitions for ThreadPool           *
 *			   Based on the Simple Threadpool from				    *
 *			   C++ Concurrency in Action 2nd Edition				*
 *Compiler: MSVC v142                                               *
 ********************************************************************/

#include "ThreadPool.hpp"
#include "Utilities.hpp"

const unsigned DEFAULTTHREADPOOLSIZE = 2;

/*
Purpose: Checks the queue for work, if none is available the thread yields
Accepts: nothing
Returns: nothing
*/

using namespace std::chrono_literals;

void ThreadPool::work()
{
	while (!done || !work_queue.empty()) //ensure that if there is any work left on the queue it gets finished
	{
		Task* task = work_queue.try_pop();
		if (task != nullptr)
		{
			task->run();
			delete task;
		}
		else
		{
			std::this_thread::sleep_for(1s);
			//std::this_thread::yield();
		}
	} 
}

/*
Purpose: Constructs the threadpool with all the initial values
Accepts: nothing
Returns: The newly created threadpool
*/
ThreadPool::ThreadPool() : done(false)
{
	unsigned numThreads = std::thread::hardware_concurrency(); //gets the number of available hardware threads
	if (numThreads == 0) //fallback if hardware_concurrency() fails to properly return a value
	{
		numThreads = DEFAULTTHREADPOOLSIZE;
	}

	try
	{
		for (unsigned i = 0; i < numThreads; ++i)
		{
			threads.push_back(std::thread(&ThreadPool::work, this));
		}
		
	}
	catch (...)
	{
		done = true;
		throw;
	}
}

/*
Purpose: Cleans up the thread and ensures any threads that are still executing finish up
Accepts: nothing
Returns: nothing
*/
ThreadPool::~ThreadPool()
{
	done = true;
	for (std::thread& t : threads)
	{
		if (t.joinable())
		{
			t.join();
		}
	}
}

/*
Purpose: Submits new task to the work queue
Accepts: A pointer to a task
Returns: nothing
*/
void ThreadPool::submitTask(Task* task)
{
	if (done) //prevent more work after the threadpool is closed
	{
		SendMessage("Threadpool is closed for the day! No more work!\n");
	}
	else
	{
		if (task != nullptr)
		{
			work_queue.push(task);
		}
	}
}

/*
Purpose: Sets done to true, letting the threads know that no more work is coming, and waits for them to finish any existing work they've started
Accepts: nothing
Returns: nothing
*/
void ThreadPool::finishWork()
{
	done = true;
	for (std::thread& t : threads)
	{
		if (t.joinable())
		{
			t.join();
		}
	}
}