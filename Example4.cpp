#include<iostream>
#include<string>

#include "Semaphore.h"

Semaphore sem(2,true,true);	// Sample semaphore with strict and fairness settings to true. only 2 permits available.

using namespace std;

int main(int argc, char* argv[])
{
	// First try to acquire the permits on the semaphore.
	sem.acquire(2);

	// Print the current status of the semaphore.
	sem.printCurrentPermitsInfo();
	sem.printQueuedThreadsInfo();
	std::cout << sem.toString() << std::endl;

	// Now attempt tryAcquire which should fail because we are strict and we dont have any permits left to acquire.only current thread can release permits.
	bool acquireStatus=sem.tryAcquire();
	std::cout << "tryAcquire status = " << acquireStatus << std::endl;

	// Now attempt acquire with wait semantics. It should also fail since it cant get anything. Wait for 500 msecs.
	acquireStatus=sem.tryAcquire(500);
	std::cout << "tryAcquire with wait status = " << acquireStatus << std::endl;

	// Now release the permits back to Semaphore.
	sem.release(2);
	sem.printCurrentPermitsInfo();
	sem.printQueuedThreadsInfo();
	std::cout << sem.toString() << std::endl;

	// Now attempt tryAcquire which should fail because we are strict and we dont have any permits left to acquire.only current thread can release permits.
	acquireStatus=sem.tryAcquire();
	std::cout << "tryAcquire status = " << acquireStatus << std::endl;

	// Now attempt acquire with wait semantics. It should also fail since it cant get anything. Wait for 500 msecs.
	acquireStatus=sem.tryAcquire(500l);
	std::cout << "tryAcquire with wait status = " << acquireStatus << std::endl;

	// Print as much info as possible now.
	sem.printCurrentPermitsInfo();
	sem.printQueuedThreadsInfo();
	std::cout << sem.toString() << std::endl;

	exit(0);
}
