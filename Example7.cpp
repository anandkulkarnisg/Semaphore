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
  cout<<message<<endl;
}

void awaitForPermits(const unsigned int& permits)
{
  try{
    sem.acquire(permits);
    printCout("Successfully acquired the permits = "+to_string(permits));
  }		
  catch(const exception& e){
    cout<<e.what()<<endl;	
  }
}

void releasePermits(const unsigned int& permits, const long& waitTimeMilliSecs)
{
  try{ 
    this_thread::sleep_for(chrono::milliseconds(waitTimeMilliSecs));
    sem.release(permits);
    printCout("Successfully release the permits = "+to_string(permits));
  }
  catch(const exception& e){
    cout<<e.what()<<endl;
  }
}

void dumpStatus()
{
  sem.printQueuedThreadsInfo();
  sem.printCurrentPermitsInfo();
  cout<<sem.toString()<<endl;
}

int main(int argc, char* argv[])
{
  // Spin off a thread pool where each one waits for successively incremental permits.
  vector<thread> acquirePool;
  acquirePool.reserve(poolSize);
  for(unsigned int i=1; i<=poolSize; ++i)
    acquirePool.emplace_back(thread(&awaitForPermits,i));

  // Now as expected they should all end up queuing for permits as there are no permits for semaphore to start with. Let us check the after waiting a little.
  this_thread::sleep_for(chrono::seconds(2));

  // Let us print the status of the semaphore. 
  dumpStatus();

  // First let us drain the semaphore and bring it to clean status.
  sem.drainPermits();

  // Now let us try and release the permits via another acquirePool one by one.
  vector<thread> releasePool;
  releasePool.reserve(poolSize);
  for(unsigned int i=poolSize; i>=1; --i)
    acquirePool.emplace_back(thread(&releasePermits,i,i*100));

  // Now join the pools.
  for_each(acquirePool.begin(), acquirePool.end(), [&](thread& threadItem) { threadItem.join(); });
  for_each(releasePool.begin(), releasePool.end(), [&](thread& threadItem) { threadItem.join(); });

  // Print the semaphore status before finish.
  cout<<"semaphore status before exit : "<<sem.toString()<<endl;

  // Finished.
  return(0);
}
