#include<iostream>
#include<string>

#include "Semaphore.h"

using namespace std;

Semaphore binarysemLock(1,true,true);
const int demoLoopCount = 100;

void printMessage()
{
	int loopCount=0;
	while(loopCount<demoLoopCount)
	{
		binarysemLock.acquire();
		cout << "I am currently in thread id =" << this_thread::get_id() << " with a count = " << loopCount << endl;
		binarysemLock.release();
		++loopCount;
	}
}

int main(int argc, char* argv[])
{
	// Implement a binary semaphore which acts like a exclusive lock.
	// Each thread will print its line alternatively after the other without overwriting as semaphore acts perfectly like a Lock.

	thread t1(&printMessage);
	thread t2(&printMessage);

	t1.join();
	t2.join();
	
	return(0);
}
