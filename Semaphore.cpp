#include<iostream>
#include<string>

#include "Semaphore.h"

using namespace std;

// First implement the Semphore constructor. We have only a single constructor which takes initial permits. Mode of fairness [ default to false ]. Mode of strictness [ default to false ].
Semaphore::Semaphore(const int& initialPermits, const bool& fair, const bool& strict) : m_permits(initialPermits), m_fair(fair), m_strict(strict)
{
	if(strict && initialPermits<0)
		throw IllegalArgumentException();
	m_name = getName();
}

// Implement simple method to get semaphore name. We dont want to keep deriving this repetative calls of toString().
string Semaphore::getName()
{
	const void * address = static_cast<const void*>(this);
	stringstream stream;
	stream << address;
	string result = stream.str();
	return(result);
}

// Implementation of getThreadId Method. Returns a string of thread-id number.
string Semaphore::getThreadId()
{
	auto myid = this_thread::get_id();
	stringstream ss;
	ss << myid;
	string resultString = ss.str();
	return(resultString);
}

// Implementation of the upsertMap Function that manages the thread information and permits it is holding in a map. Any call to this is with in the Locked state. Dont try to lock inside this again!
void Semaphore::upsertMap(const string& threadId, const int& permits)
{
	auto iter = m_map.find(threadId);
	if(iter != m_map.end())
		iter->second +=permits;
	else
		m_map[threadId]=permits;
}

// Implementation of evictThreadIdFromQueue Method. This is bit cumbersome but no other choice. No fairness policy is set. current thread was queued for permits. It has got them. It has to be evicted
// from the waiting threads queue.

void Semaphore::evictThreadIdFromQueue(const string& threadId)
{
	auto iter=find_if(m_queue.begin(), m_queue.end(), [&](const tuple<string, int>& item){ return(get<0>(item)==threadId); });
	if(iter!=m_queue.end())
		m_queue.erase(iter);
}

// Implement updateOrDeleteMap Method. It is only applicable in strict mode. In case the second parameter is +ve entry is alive and updated in map, if zero then it is deleted since it has no alive permits.
// This is already called from a Locked function , so dont lock inside again!
void Semaphore::updateOrDeleteMap(const string& threadId, const int& permitCount)
{
	auto iter=m_map.find(threadId);
	if(permitCount>0)
		iter->second=permitCount;		
	else
		m_map.erase(iter);					
}

// Next We implement the acquire method as core Internal utlity. This is called by both single and bulk acquire methods.The acquire blocks till a permit is available. Else simply returns once permits available.
void Semaphore::acquireInternal(const int& permits = 1)
{
	// Edge case : if permits is zero. simply return. We dont acquire zero permits.
	if(permits==0)
		return;

	// First let us take a lock to proceed further.
	unique_lock<mutex> exclusiveLock(m_mutex);
	string threadId = getThreadId();

	// Now check if permits are available and waitQueue is empty. If so simply reduce its count directly.
	if(m_permits-permits>=0 && m_queue.size()==0)
	{
		m_permits-=permits;
		// Now if we are in mode of strictness. Add this info to the map.
		if(isStrict())
			upsertMap(threadId, permits);
	}
	else
	{
		m_queue.push_back(make_tuple(threadId,permits));
		if(isFair())
		{
			m_cond.wait(exclusiveLock, [&](){ return((m_permits-permits)>=0 && get<0>(*m_queue.begin()) == threadId); });
			m_queue.pop_front();
		}
		else
		{
			m_cond.wait(exclusiveLock, [&](){ return((m_permits-permits)>=0); });
			evictThreadIdFromQueue(threadId);
		}	

		m_permits-=permits;

		// Now if we are in mode of strictness. Add this info to the map.
		if(isStrict())
			upsertMap(threadId, permits);
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

// Implement drain permits. Acquires and returns all permits that are immediately available, or if negative permits are available, releases them. Upon return, zero permits are available.
int Semaphore::drainPermits()
{
	// Edge case : If there are no permits do nothing and return.
	int returnPermits = 0;
	if(m_permits==0)
		return(returnPermits);

	unique_lock<mutex> exclusiveLock(m_mutex);

	// Weather strict or relaxed mode if permits are available then acquire and release them. If zero permits are available them return zero.
	// In relaxed mode permits can go -ve , in which case add permits and set it to zero.

	if(m_permits>=0)
	{
		returnPermits=m_permits;
		m_permits=0;
		return(returnPermits);
	}

	// In relaxed mode set permits to zero if -ve permits are available.
	if(!isStrict())
	{
		if(m_permits<0)
		{
			returnPermits=abs(m_permits);
			m_permits=0;
		}
	}
	return(returnPermits);
}

// Implement the method getQueuedThreads. Return a vector of string each of which is a threadId that is currently queued waiting for a permit to become available.
deque<tuple<string,int>> Semaphore::getQueuedThreads()
{
	unique_lock<mutex> exclusiveLock(m_mutex);
	return(m_queue);
}

// Implement the method printQueuedThreadsInfo. This will print in nice format the threads queued for acquire and count they are waiting for.
void Semaphore::printQueuedThreadsInfo()
{
	deque<tuple<string,int>> resultQueue = getQueuedThreads();
	for(const auto& iter : resultQueue)
		cout << "Waiting Thread id = " << get<0>(iter) << ", acquireCount = " << get<1>(iter) << endl;
}

// Implement getQueueLength Method. Returns the length of the Queue of threads that are currently waiting for the permit to become available.
int Semaphore::getQueueLength()
{
	unique_lock<mutex> exclusiveLock(m_mutex);
	return(m_queue.size());
}

// Implement method hasQueuedThreads. This tells if there are any threads queued for waiting to get permits.
bool Semaphore::hasQueuedThreads()
{
	unique_lock<mutex> exclusiveLock(m_mutex);
	if(m_queue.size()>0)
		return(true);
	return(false);
}

// Implement isFair Method. Indicates if FIFO queuing is used internally to queue the waiting threads for the permit(s).
bool Semaphore::isFair()
{
	return(m_fair);
}

// Implement isStrict Method. Indicates if strict mode is on. In strict mode there is concept of ownership of permits for each thread. only a thread that has permits can release them.
bool Semaphore::isStrict()
{
	return(m_strict);
}

// Implement releaseInternal Method. Tries to release the permit(s) that a thread has acquired.Throws exception in case of strict mode where a release is tried without having acquired any permit.
void Semaphore::releaseInternal(const int& permits=1)
{
	// Edge case : if permits is zero. simply return. We dont acquire zero permits.
	if(permits==0)
		return;

	// First of all if the permits is a -ve number then throw IllegalArgumentException. 
	if(permits<0)
		throw NegativeReleasePermitsException();

	// Take a lock and start the work.
	unique_lock<mutex> exclusiveLock(m_mutex);
	string threadId=getThreadId();

	// Now if we are in relaxed mode then simply release the permits without checking anything further.
	if(!isStrict())
	{
		m_permits+=permits;
		// Notify the waiting acquire threads. Unlock before notification else waiting threads will never get the Lock!
		exclusiveLock.unlock();
	}
	else
	{
		auto iter=m_map.find(threadId);
		if(iter != m_map.end())
		{	
			int permitCount=iter->second;
			if(permits>permitCount)
			{
				throw ExcessReleasePermitsException();				
			}				
			else
			{
				m_permits+=permits;
				updateOrDeleteMap(threadId,permitCount-permits);
				exclusiveLock.unlock();
			}
		}
		else
		{
			throw IllegalPermitsReleaseException();
		}
	}

	// This is actually interesting. If the mode is fair then we signal only if we know that the permits that we have released will serve the first item in the queue.
	// We also need to signal in fair mode as long as wait queue exists and we can serve the front item currently. This fixes a rare bug where notify may miss out and some acquire may keep waiting.
	if(isFair())
	{
		while(hasQueuedThreads() && get<1>(*m_queue.begin()) <=m_permits)
		{
			m_cond.notify_all();
		}
	}
	else
	{
		m_cond.notify_all();
	}
}

// Implement release Method. This simply wraps up around the releaseInternal Method.
void Semaphore::release()
{
	releaseInternal();
}

// Implement release Method for bulk permits. This simply wraps up around the releaseInternal Method.
void Semaphore::release(const int& permits)
{
	releaseInternal(permits);
}

// This implemements the toString Method. It currently indicates the status of the Semaphore with as much detail as possible in single Line!. Additional details required for debugging
// Must be implemented via getQueuedThreads or printCurrentPermitsInfo Methods.
string Semaphore::toString()
{
	unique_lock<mutex> exclusiveLock(m_mutex);
	string returnString = " ==> [ Semaphore Name = Semaphore." + m_name;	
	returnString += ", permits available = " + to_string(m_permits);
	returnString += ", fairness = " + to_string(m_fair);
	returnString += ", strict = " + to_string(m_strict);
	returnString += ", waiting threads queue length = " + to_string(m_queue.size()) + ". ]";
	return(returnString);
}

// This implements the tryAcquire Method. The tryAcquire method aims to return if any immediate permits are available [ wihtout blocking or queuing ]. They do not honor any fairness.
// As their need is immediate. Since they do not queue only map needs update in case of Strict Mode. If they acquire permits then return true else false.
// First we implement tryAcquireInternal and then wrap the public call around it for clean implementation.

bool Semaphore::tryAcquireInternal(const int& permits=1, const bool& timeOutNeeded = false, const long& waitTimeMilliSecs=0)
{
	// Edge case : if permits is zero. simply return. We dont acquire zero permits.
	if(permits==0)
		return(true);

	// First of all check if we are in wait mode or not.
	string threadId = getThreadId();
	if(!timeOutNeeded)
	{
		if(m_permits-permits>=0)
		{
			unique_lock<mutex> exclusiveLock(m_mutex, defer_lock);
			if(exclusiveLock.try_lock())
			{
				if(m_permits-permits>=0)	// DCLP because after acquiring mutex we dont know if m_count is still valid.
				{
					m_permits-=permits;
					if(isStrict())
						upsertMap(threadId, permits);
					return(true);
				}
			}
			else
			{
				return(false);
			}
		}
		else
		{
			return(false);
		}
	}
	else
	{
		if(waitTimeMilliSecs == 0)
			return(false);
		auto startTime = chrono::high_resolution_clock::now();
		auto endTime =  chrono::high_resolution_clock::now();
		auto duration = 0;
		while(duration<=waitTimeMilliSecs)
		{
			if(m_permits-permits>=0)
			{
				unique_lock<mutex> exclusiveLock(m_mutex, defer_lock);
				if(exclusiveLock.try_lock())
				{
					if(m_permits-permits>=0)    // DCLP because after acquiring mutex we dont know if m_count is still valid.
					{
						m_permits-=permits;
						if(isStrict())
							upsertMap(threadId, permits);
						return(true);	
					}
					else
					{
						exclusiveLock.unlock();	// Else we have a possibility of same thread trying to Lock the mutex multiple times.
					}
				}		
			}	
			endTime =  chrono::high_resolution_clock::now();
			duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();				
		}	
	}
	return(false);
}

// Now we Implement the tryAcquire Method which is wrapped around tryAcquireInternal.
bool Semaphore::tryAcquire()
{
	bool returnStatus=tryAcquireInternal();
	return(returnStatus);
}

// Now we Implement the tryAcquire Method which is wrapped around tryAcquireInternal Method.
bool Semaphore::tryAcquire(const int& permits)
{
	bool returnStatus=tryAcquireInternal(permits,false,0);
	return(returnStatus);
}

// Now we Implement tryAcquire which wants 1 permit and is willing to wait for it.
bool Semaphore::tryAcquire(const long& waitTimeInMilliSecs)
{
	bool returnStatus=tryAcquireInternal(1,true,waitTimeInMilliSecs);
	return(returnStatus);
}

// Now we Implement tryAcquire which wants more than 1 permit and is willing to wait for it.
bool Semaphore::tryAcquire(const int& permits, const long& waitTimeInMilliSecs)
{
	bool returnStatus=tryAcquireInternal(permits,true,waitTimeInMilliSecs);
	return(returnStatus);
}

// Now we Implement printCurrentPermitsInfo Method. It prints information only if mode is strict. Only in strict mode we note down which thread is holding how many permits.
// This function is for debug purpose. It has no meaning in relaxed mode since in relaxed mode there is no tracking of which thread is holding how many permits. Any thread can acquire or release
// as many permits as they wish in relaxed mode.
void Semaphore::printCurrentPermitsInfo()
{
	if(isStrict())
	{
		unique_lock<mutex> exclusiveLock(m_mutex);
		for(const auto& iter : m_map)
			cout << "Thread id = " << iter.first << ", permitCount = " << iter.second << endl;
	}
}

