#include<algorithm>
#include<chrono>
#include<condition_variable>
#include<deque>
#include<iostream>
#include<map>
#include<mutex>
#include<sstream>
#include<string>
#include<thread>
#include<tuple>
#include<vector>

#include "SemaphoreExceptions.h"

#ifndef Semaphore_H
#define Semaphore_H

class Semaphore
{
	private:
		int m_permits = 0;										// This is the initial number of permits assigned to the semaphore.It can increase or decrease later on while usage.
		std::string m_name;										// Name specific to the Semaphore.
		std::mutex m_mutex;										// This is used to manage the synchronization of the threads.		
		std::condition_variable m_cond;							// It is used for signalling purpose between threads.
		bool m_fair = false;									// Is is used to indicate if the Semaphore is fair in dealing with queued threads/requests. by default is it false.		
		bool m_strict = false;									// This is used to implement a strict ownership semantic.If this is set only a thread that has acquired a permit can release [ same amount ].
		std::deque<std::tuple<std::string, int>> m_queue;		// This is used to queue incoming requests. It has thread-id and two flags that indicate request is bulk/one permit and amount needed.
		std::map<std::string, int> m_map;					    // This is only used in strict mode. Indicates which thread is holding how many permits and what kind of permit it is.
		std::string getThreadId();								// This is private method that returns threadId as string.
		void acquireInternal(const int&);						// This is internal implementation of acquire serving both single and bulk acquire of permits.
		void releaseInternal(const int&);						// This is internal implementation of release serving both single and bulk release of permits.
		void upsertMap(const std::string&, const int&);			// This is internal function to update m_map in strict mode.
		void updateOrDeleteMap(const std::string&, const int&); // This is internal function to either update or delete an entry during release in strict mode.
		void evictThreadIdFromQueue(const std::string&);		// This is internal function to remove a threadId from queue once the permits are available for thread and is not waiting anymore.
		bool tryAcquireInternal(const int&, const bool&, const long&);	// This internal function implements tryAcquire approach. read Implementation for more details.
		std::string getName();									// This is to derive the Latch name during construction.

	protected:
		void reducePermits(const int&);							// This Shrinks the number of available permits by the indicated reduction.

	public:
		Semaphore(const int&, const bool& = false, const bool& = false);		// This is the only constructor available.
		Semaphore(const Semaphore&) = delete;					// copy deleted.
		Semaphore& operator=(const Semaphore&) = delete;		// assignment deleted.

		void acquire();											// Ask for a permit blocking in nature.	
		void acquire(const int&);								// Ask for a bulk permit blocking in nature.
		int availablePermits();									// Indicate available number of permit currently. Best case estimate as it can change any moment after the value is returned.
		int drainPermits();										// drain all the existing available permits and use them. If -ve permits are available then release them [ till it becomes zero ].
		std::deque<std::tuple<std::string, int>> getQueuedThreads();			// This returns a list of strings that indicate the thread-ids currently queued for the Semaphore.
		int getQueueLength();									// This indicates the length of the queue waiting for permits.
		bool hasQueuedThreads();								// Indicates if threads are waiting for permits.
		bool isFair();											// Is there scheduling of incoming request for permits into a FIFO queue ?
		bool isStrict();										// Is the Semaphore strict ? if so only threads acquring permits can release them [ in same amount ]. No permit resizing allowed than initial.
		void release();											// release a permit back to the Semaphore.
		void release(const int&);								// rleease permits back in bulk to Semaphore.
		std::string toString();									// Print a current status and info of Semaphore in friendly fashion. Helpful for debugging.
		bool tryAcquire();										// Acquires a permit from this semaphore, only if one is available at the time of invocation.
		bool tryAcquire(const long&);							// Acquires a permit from this semaphore, if one becomes available within the given waiting time else timesout.
		bool tryAcquire(const int&);							// Acquires the given number of permits from this semaphore, only if all are available at the time of invocation.
		bool tryAcquire(const int&, const long&);				// Acquires the given number of permits from this semaphore, if all become available within the given waiting time.
		void printCurrentPermitsInfo();							// only available in strict mode of ownership. Else not available.
		void printQueuedThreadsInfo();							// This prints out in nice format queued threads and the count acquire for which they are waiting.
};

#endif

