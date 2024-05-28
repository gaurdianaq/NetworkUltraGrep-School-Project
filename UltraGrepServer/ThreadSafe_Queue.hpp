/********************************************************************
 *Author: Anthony Williams & Evan Tatay-Hinds                       *
 *Date: November 8th 2019                                           *
 *Description: Contains class declaration for threadsafe_queue      *
 *			   Based on the thread safe queue from				    *
 *			   C++ Concurrency in Action 2nd Edition				*
 *Compiler: MSVC v142                                               *
 ********************************************************************/

#ifndef THREADSAFE_QUEUE
#define THREADSAFE_QUEUE

#include <mutex>
#include <queue>

//implementation of the STL queue that ensures a lock is placed on the queue whenever accesing the queue
template<typename T>
class threadsafe_queue
{
private:
	mutable std::mutex mut;
	std::queue<T*> data_queue;
	std::condition_variable data_cond;
public:
	threadsafe_queue()
	{}

	/*
	Purpose: Pushes a new item onto the queue, and notifies one thread that work is available
	Accepts: New value
	Returns: nothing
	*/
	void push(T* new_value)
	{
		std::lock_guard<std::mutex> lk(mut);
		data_queue.push(new_value);
		data_cond.notify_one();
	}

	/*
	Purpose: Attempts to return the first item in the queue and then pop it
	Accepts: nothing
	Returns: pointer to first item in the queue if something is there
	*/
	T* try_pop() 
	{
		std::lock_guard<std::mutex> lk(mut);
		if (data_queue.empty())
			return nullptr;
		T* res = data_queue.front();
		data_queue.pop();
		return res;
	}

	/*
	Purpose: Checks if the queue is empty
	Accepts: nothing
	Returns: bool representing whether it's empty or not
	*/
	bool empty() const
	{
		std::lock_guard<std::mutex> lk(mut);
		return data_queue.empty();
	}
};

#endif // !THREADSAFE_QUEUE