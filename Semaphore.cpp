#include<iostream>
#include<string>

#include "Semaphore.h"

using namespace std;

// First implement the Semphore constructor. We have only a single constructor which takes initial permits. Mode of fairness [ default to false ]. Mode of strictness [ default to false ].
Semaphore::Semaphore(const int& initialPermits, const bool& fair = false, const bool& strict = false) : m_permits(initialPermits), m_fair(fair), m_strict(strict)
{

}

// Implementation of getThreadId Method. Returns a std::string of thread-id number.
string Semaphore::getThreadId()
{
	auto myid = this_thread::get_id();
	stringstream ss;
	ss << myid;
	string resultString = ss.str();
	return(resultString);
}

// Implementation of the updateMap Function that manages the thread information and permits it is holding in a map. Any call to this is with in the Locked state. Dont try to lock inside this again!
void Semaphore::updateMap(const std::string& threadId, const int& permits)
{
	auto iter = m_map.find(threadId);
	if(iter != m_map.end())
		iter->second +=permits;
	else
		m_map[threadId]=permits;
}

// Next We implement the acquire method as core Internal utlity. This is called by both single and bulk acquire methods.The acquire blocks till a permit is available. Else simply returns once permits available.
void Semaphore::acquireInternal(const int& permits = 1)
{
	// First let us take a lock to proceed further.
	unique_lock<mutex> exclusiveLock(m_mutex);
	std::string threadId = getThreadId();

	// Now check if permits are available. If so simply reduce its count.
	if(m_permits-permits>=0)
	{
		m_permits-=permits;
		// Now if we are in mode of strictness. Add this info to the map.
		if(isStrict())
			updateMap(threadId, permits);
	}
	else
	{
		if(isFair())
		{
			m_queue.push_back(make_tuple(threadId,1));
			m_cond.wait(exclusiveLock, [&](){ return((m_permits-permits)>0 && get<0>(m_queue[0]) == threadId); });
			m_queue.pop_front();
		}
		else
		{
			m_cond.wait(exclusiveLock, [&](){ return((m_permits-permits)>=0); });
		}	

		m_permits-=permits;

		// Now if we are in mode of strictness. Add this info to the map.
		if(isStrict())
			updateMap(threadId, permits);
	}
}

// Implement the acquire method as a wrapper call to the acquireInternal method. This is sufficient.
void Semaphore::acquire()
{
	acquireInternal();
}

// Implement the acquire bulk permits as a wrapper to the acquireInternal method. This is sufficient.
void Semaphore::acquire(const int& permits)
{
	acquireInternal(permits);

}

// Implement the method to indicate how many permits are available currently.
int Semaphore::availablePermits()
{
	unique_lock<mutex> exclusiveLock(m_mutex);
	return(m_permits);
}

