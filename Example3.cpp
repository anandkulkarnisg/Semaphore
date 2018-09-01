#include<iostream>
#include<string>
#include<mutex>
#include<algorithm>
#include<vector>

#include "Semaphore.h"

using namespace std;

Semaphore test(10,true,true);
std::mutex cout_mutex;

void printOut(const std::string& str1, const std::string& str2, const std::thread::id& id)
{
	std::unique_lock<std::mutex> lock(cout_mutex);
	std::cout << str1 << id << str2 << std::endl;
}

void acquireThreadFunc(const int& permits, const int& waitCount)
{
	try
	{
		test.acquire(permits);
		printOut("I am currently running from thread id = ", ". I have successfully acquired permits of count = " + to_string(permits), std::this_thread::get_id());
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	std::this_thread::sleep_for(std::chrono::seconds(waitCount));
	test.release(2);

	std::this_thread::sleep_for(std::chrono::seconds(waitCount));
	test.release(3);
}

void waitThreadFunc(const int& permits)
{
	try
	{	
		test.acquire(permits);
		printOut("I am currently running from thread id = ", ". I have successfully acquired permits of count = " + to_string(permits), std::this_thread::get_id());
		test.printQueuedThreadsInfo();
		test.printCurrentPermitsInfo();
		std::cout << test.toString() << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

int main(int argc, char* argv[])
{
	// Test Case 1 : Spin a thread that will acquire all the permits and wait for given period of time and then release the permit.
	std::thread t1(&acquireThreadFunc,10,5);

	// Spin two threads that will wait for the permits to become available and then release them one by one in the first thread.
	std::thread t2(&waitThreadFunc,2);	
	std::thread t3(&waitThreadFunc,3);

	// Test that the threads have queued. Verify their order of release.
	std::this_thread::sleep_for(std::chrono::seconds(2));

	// Finally join them and exit.
	t1.join();
	t2.join();
	t3.join();

	return(0);
}
