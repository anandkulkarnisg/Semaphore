#include<iostream>
#include<string>

#include "Semaphore.h"

using namespace std;

int main(int argc, char* argv[])
{

  // Test Case 1 : Just acquire and keep printing the current permits info and status of the semaphore.
  Semaphore test(10,true,true);
  for(unsigned int i=0; i<10; ++i)
  {
    test.acquire();
    test.printCurrentPermitsInfo();
    cout << "Status of the Semaphore is " << test.toString() << endl;	
  }

  // Test case 2 : Release back the permits and test the same settings.
  for(unsigned int i=0; i<10; ++i)
  {	
    test.release();
    test.printCurrentPermitsInfo();
    cout << "Status of the Semaphore is " << test.toString() << endl;
  }			

  // Test case 3 : Now try to reduce the permit again , it should throw an exception because the current thread does not have any permits acquired leftover.
  try
  {
    test.release();
  }
  catch(const exception& e)
  {
    cout << e.what() << endl;
  }

  // Test case 4 : Attempt now to try and release -ve permits. Should recieve an exception.
  try
  {
    test.release(-100);
  }
  catch(const exception& e)
  {
    cout << e.what() << endl;
  }

  // Test case 5 : Attempt now to bulk acquire some permits again.Should work.
  try
  {	
    test.acquire(4);
    test.printCurrentPermitsInfo();
    cout << "Status of the Semaphore is " << test.toString() << endl;
  }
  catch(const exception& e)
  {
    cout << e.what() << endl;
  }

  // Test case 6 : Attempt now a clean bulk release of the same permits.Should work.
  try
  {
    test.release(4);
    test.printCurrentPermitsInfo();
    cout << "Status of the Semaphore is " << test.toString() << endl;
  }
  catch(const exception& e)
  {
    cout << e.what() << endl;
  }

  // Test case 7 : Now attempt bulk acquire of more capacity than initially assigned. Should block the current thread.
  try
  {
    cout << "Press CNTRL-C to exit the progream. The current thread acquire is waiting in the queue" << endl;
    test.acquire(11);
    test.printCurrentPermitsInfo();
    cout << "Status of the Semaphore is " << test.toString() << endl;
  }
  catch(const exception& e)
  {
    cout << e.what() << endl;
  }

  return(0);
}
