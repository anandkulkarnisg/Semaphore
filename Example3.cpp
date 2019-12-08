#include<iostream>
#include<string>
#include<mutex>
#include<algorithm>
#include<vector>

#include "Semaphore.h"

using namespace std;

Semaphore test(10, true, true);
mutex cout_mutex;

void printOut(const string& str1, const string& str2, const thread::id& id)
{
	unique_lock<mutex> lock(cout_mutex);
	cout << str1 << id << str2 << endl;
}

void acquireThreadFunc(const int& permits, const int& waitCount)
{
	try
	{
		test.acquire(permits);
		printOut("I am currently running from thread id = ", ". I have successfully acquired permits of count = " + to_string(permits), this_thread::get_id());
	}
	catch(const exception& e)
	{
		cout << e.what() << endl;
	}
	this_thread::sleep_for(chrono::seconds(waitCount));
	test.release(2);

	this_thread::sleep_for(chrono::seconds(waitCount));
	test.release(3);
}

void waitThreadFunc(const int& permits)
{
	try
	{	
		test.acquire(permits);
		printOut("I am currently running from thread id = ", ". I have successfully acquired permits of count = " + to_string(permits), this_thread::get_id());
		test.printQueuedThreadsInfo();
		test.printCurrentPermitsInfo();
		cout << test.toString() << endl;
	}
	catch(const exception& e)
	{
		cout << e.what() << endl;
	}
}

// This code example may get blocked at t1 thread indefinitely. This is not a bug. because thread t2 and t3 jump ahead and acquire 2,3 permits and only 5 are left from 10.
// Next t1 attempts to acquire 10 permits but only 5 are present.so it is blocked!! This is bug at application layer. It demonstrates that thread safe data structures can
// still be mis used at application level.

int main(int argc, char* argv[])
{
	// Test Case 1 : Spin a thread that will acquire all the permits and wait for given period of time and then release the permit.
	thread t1(&acquireThreadFunc, 10, 5);

	// Spin two threads that will wait for the permits to become available and then release them one by one in the first thread.
	thread t2(&waitThreadFunc, 2);	
	thread t3(&waitThreadFunc, 3);

	// Test that the threads have queued. Verify their order of release.
	this_thread::sleep_for(chrono::seconds(2));

	// Finally join them and exit.
	t1.join();
	t2.join();
	t3.join();

	return(0);
}
