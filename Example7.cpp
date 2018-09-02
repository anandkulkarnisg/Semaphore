#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<mutex>

#include "Semaphore.h"

using namespace std;

Semaphore sem(-10);		// A relaxed and non fair semaphore.
const unsigned int poolSize = 10;
mutex cout_mutex;

void printCout(const string& message)
{
	lock_guard<mutex> lock(cout_mutex);
	cout << message << endl;
}

void awaitForPermits(const unsigned int& permits)
{
	try
	{
		sem.acquire(permits);
		printCout("Successfully acquired the permits = "+to_string(permits));
	}		
	catch(const exception& e)
	{
		cout << e.what() << endl;	
	}
}

void releasePermits(const unsigned int& permits, const long& waitTimeMilliSecs)
{
	try
	{
		this_thread::sleep_for(chrono::milliseconds(waitTimeMilliSecs));
		sem.release(permits);
		printCout("Successfully release the permits = "+to_string(permits));
	}
	catch(const exception& e)
	{
		cout << e.what() << endl;
	}
}

void dumpStatus()
{
	sem.printQueuedThreadsInfo();
	sem.printCurrentPermitsInfo();
	cout << sem.toString() << endl;
}

int main(int argc, char* argv[])
{
	// Spin off a thread pool where each one waits for successively incremental permits.
	vector<thread> threadPool;
	threadPool.reserve(poolSize);
	for(unsigned int i=1; i<=poolSize; ++i)
		threadPool.emplace_back(thread(&awaitForPermits,i));

	// Now as expected they should all end up queuing for permits as there are no permits for semaphore to start with. Let us check the after waiting a little.
	this_thread::sleep_for(chrono::seconds(2));

	// Let us print the status of the semaphore. 
	dumpStatus();

	// First let us drain the semaphore and bring it to clean status.
	sem.drainPermits();

	// Now let us try and release the permits via another threadPool one by one.
	vector<thread> releasePool;
	releasePool.reserve(poolSize);		
	for(unsigned int i=poolSize; i>=1; --i)
		threadPool.emplace_back(thread(&releasePermits,i,i*100));

	// Now join the pools.
	std::for_each(threadPool.begin(), threadPool.end(), [&](thread& threadItem) { threadItem.join(); });
	std::for_each(releasePool.begin(), releasePool.end(), [&](thread& threadItem) { threadItem.join(); });

	// Finished.
	return(0);

}