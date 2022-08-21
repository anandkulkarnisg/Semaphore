#include<iostream>
#include<string>

#include "Semaphore.h"

using namespace std;

void printMessage(Semaphore& binarySemLock, const int& demoLoopCount)
{
  int loopCount=0;
  while(loopCount<demoLoopCount)
  {
    binarySemLock.acquire();
    cout<<"I am currently in thread id ="<<this_thread::get_id()<<" with a count = "<<loopCount<<endl;
    binarySemLock.release();
    ++loopCount;
  }
}

int main(int argc, char* argv[])
{
  // Implement a binary semaphore which acts like a exclusive lock.
  // Each thread will print its line alternatively after the other without overwriting as semaphore acts perfectly like a Lock.
  
  // Create a semaphore locally here and pass to threads.
  Semaphore binarySemLock(1, true, true);
  constexpr int demoLoopCount=100;

  thread t1(&printMessage, std::ref(binarySemLock), demoLoopCount);
  thread t2(&printMessage, std::ref(binarySemLock), demoLoopCount);

  t1.join();
  t2.join();

  return(0);
}
