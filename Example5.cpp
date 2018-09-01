#include<iostream>
#include<string>

#include "Semaphore.h"

using namespace std;

Semaphore sem(-10, false, false);		// A relaxed semaphore and without fairness set.

int main(int argc, char* argv[])
{
	// First of all print the current summary of semaphore.
	sem.printCurrentPermitsInfo();
	sem.printQueuedThreadsInfo();
	cout << sem.toString() << endl;

	// Now attempt to drain permits.
	int result=sem.drainPermits();
	cout << "permit drain result = " << result << endl;

	sem.printCurrentPermitsInfo();
	sem.printQueuedThreadsInfo();
	cout << sem.toString() << endl;

}

