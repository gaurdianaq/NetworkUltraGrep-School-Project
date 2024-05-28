/********************************************************************
 *Author: Anthony Williams & Evan Tatay-Hinds                       *
 *Date: November 8th 2019                                           *
 *Description: Contains class declaration for ThreadPool            *
 *			   Based on the Simple Threadpool from				    *
 *			   C++ Concurrency in Action 2nd Edition				*
 *Compiler: MSVC v142                                               *
 ********************************************************************/

#ifndef _THREAD_POOL
#define _THREAD_POOL

#include <thread>
#include <atomic>
#include "ThreadSafe_Queue.hpp"
#include "Task.hpp"

//Manages the lifecycle of the threads and assigns them work
class ThreadPool
{
	std::atomic<bool> done;
	threadsafe_queue<Task> work_queue;
	std::vector<std::thread> threads;
	void work();
public:
	ThreadPool();
	~ThreadPool();
	void submitTask(Task* task);
	void finishWork();
};

#endif // !_THREAD_POOL