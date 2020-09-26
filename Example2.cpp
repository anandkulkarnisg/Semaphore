#include<iostream>
#include<string>
#include<mutex>
#include<algorithm>
#include<vector>

#include "Semaphore.h"

using namespace std;

Semaphore test(10,true,true);
mutex cout_mutex;

void printOut(const string& str1, const string& str2, const thread::id& id)
{
  unique_lock<mutex> lock(cout_mutex);
  cout<<str1<<id<<str2<<endl;
  test.printCurrentPermitsInfo();
  cout<<test.toString()<<endl;
}

void acquireThreadFunc(const int& permits)
{
  try{
    test.acquire(permits);
    printOut("I am currently running from thread id = ", ". I have successfully acquired permits of count = " + to_string(permits), this_thread::get_id());
  }
  catch(const exception& e){
    cout<<e.what()<<endl;
  }
}

int main(int argc, char* argv[])
{
  // Test Case 1 : Attempt a set of 5 threads acquiring different amount of permits. 
  cout<<"Warning : This example can get blocked indefinitely. This is expected."<<endl;
  vector<thread> threadPool;
  threadPool.reserve(5);

  for(unsigned int i=0; i<5;++i)
    threadPool.emplace_back(thread(&acquireThreadFunc,i));	
  for_each(threadPool.begin(), threadPool.end(), [&](thread& threadItem){ threadItem.join(); });

  // Test Case 2 : Spin a thread now which wants 2 permits. It will be blocked in the queue. Later from main show the Semaphore status.
  cout<<"Now i am spawning another thread which needs 2 permits. It will block"<<endl;
  thread rawThread(&acquireThreadFunc, 2);
  this_thread::sleep_for(chrono::milliseconds(100));
  test.printCurrentPermitsInfo();
  cout << test.toString() << endl;
  test.printQueuedThreadsInfo();	
  cout<<"Please use CNTRL+C to exit. The thread is now blocked as expected."<<endl;
  rawThread.join();
  return(0);
}
