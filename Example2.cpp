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
	test.printCurrentPermitsInfo();
	std::cout << test.toString() << std::endl;
}

void acquireThreadFunc(const int& permits)
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
}

int main(int argc, char* argv[])
{
	// Test Case 1 : Attempt a set of 5 threads acquiring different amount of permits. 
	std::vector<std::thread> threadPool;
	threadPool.reserve(5);

	for(unsigned int i=0; i<5;++i)
	{
		threadPool.emplace_back(std::thread(&acquireThreadFunc,i));	
	}
	std::for_each(threadPool.begin(), threadPool.end(), [&](std::thread& threadItem){ threadItem.join(); });

	// Test Case 2 : Spin a thread now which wants 2 permits. It will be blocked in the queue. Later from main show the Semaphore status.
	std::cout << "Now i am spawning another thread which needs 2 permits. It will block" << std::endl;
	std::thread rawThread(&acquireThreadFunc,2);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	test.printCurrentPermitsInfo();
	std::cout << test.toString() << std::endl;
	test.printQueuedThreadsInfo();	
	std::cout << "Please use CNTRL+C to exit. The thread is now blocked as expected." << std::endl;
	rawThread.join();
	return(0);
}
