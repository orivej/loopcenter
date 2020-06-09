
#include "Thread.h"

Thread::Thread() {}

int Thread::Start(void)
{
   int code = pthread_create(&ThreadId_, 0, Thread::EntryPoint, this);
   return code;
}

int Thread::Run(void)
{
   Setup();
   Execute();

   return 0;
}

/*static */
void* Thread::EntryPoint(void * pthis)
{
   Thread * pt = (Thread*)pthis;
   pt->Run();

   return (void *) NULL;
}

void Thread::Setup()
{
        // Do any setup here
}

void Thread::Execute(void)
{
        // Your code goes here
}
