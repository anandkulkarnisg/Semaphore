DataStructure :Semaphore


Implementation of a Semaphore in C++ [ Conforming to Implementation interface of standard of Java ].

Author : Anand Kulkarni - 29/08/2018 - Implementation of a Semaphore trying to conform to the Java Implementation standard with additional features.
Source : https://docs.oracle.com/javase/9/docs/api/java/util/concurrent/Semaphore.html

Design Decisions:

1. The semaphore implemented in Java and in general does not have concept of ownership. Additional permits may be added and acquired as required dynamically greater than initial 
   permits supplied to the semaphore.

2. The C++ Implementation i have done has additional policy variable during construction. If the policy is strict then 
      a. No additional permits may be added by somebody dynamically and change the size of permits.
	  b. Each thread owning the permit must release it else it will leak the permit.
      c. The resizing of permits is not allowed.
      d. If a thread that does not own any permits tries to do a release then it will generate exception.

   The default policy is relaxed. This is the style of Java implementation.
      a. permit may be dynamically added by any thread and permits may increase and decrease to zero. -ve permit size is not allowed.
      b. Each thread owning permit has to release it via proper design.
      c. The reisizing of permits is allowed.
      d. anonymous thread can release permits freely even without having acquired any.

3. The fairness policy is in line with Java. requests are queued if currently no permits are available.If fairness it set then internal queue will decide and ensure no threads are starved.
   The first in the queue is served unless it is bulk permit request and such count of permits is not available.

4. A debug function is additional which allows to print at any time the current permits available and which thread is using them [ thread id is indicated ]. This is only useful for strict
   mode. In relaxed mode this info is meaningless since any thread can add and drain permits as required and there is no concept of ownership of permit by thread.

Resources :
- https://docs.oracle.com/javase/9/docs/api/java/util/concurrent/Semaphore.html
- https://stackoverflow.com/questions/17825508/fairness-setting-in-semaphore-class
- https://stackoverflow.com/questions/52108061/java-semaphore-implementation-design-related-questions
- https://stackoverflow.com/questions/7554839/how-and-why-can-a-semaphore-give-out-more-permits-than-it-was-initialized-with

