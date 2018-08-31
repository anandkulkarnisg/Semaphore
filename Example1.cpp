#include<iostream>
#include<string>

#include "Semaphore.h"

using namespace std;

int main(int argc, char* argv[])
{
	Semaphore test(10,true,true);
	test.acquire();
	test.acquire();
	test.printCurrentPermitsInfo();
	test.release();
	test.printCurrentPermitsInfo();
	test.release();
	test.printCurrentPermitsInfo();
	return(0);
}
