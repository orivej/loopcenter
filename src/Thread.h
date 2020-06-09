/* This class is an abstract thread class.  Inherit from this class, and then
   call the Start() method.  Override the Execute() method, and this will
   be the entry point of thread execution.
*/

#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

class Thread
{
   public:
      Thread();
      virtual ~Thread() {}
      int Start(void);
   protected:
      int Run(void);
      static void* EntryPoint(void*);
      virtual void Setup();
      virtual void Execute(void);
      //      void * Arg() const {return Arg_;}
      //      void Arg(void* a){Arg_ = a;}
   private:
      pthread_t ThreadId_;

};

// Portable implementation of a mutex.
class Mutex
{
 public:
  Mutex() {
    m_count = 0;
    m_threadID = 0;
    pthread_mutex_init( &m_mutex, 0 );
  }
  
  ~Mutex() { pthread_mutex_destroy( &m_mutex ); }
  
  void lock() {
    // If this thread doesn't already have the lock, wait to obtain it:
    if( !pthread_equal(m_threadID, pthread_self()) ) {
      // get lock:
      pthread_mutex_lock( &m_mutex );
      // save locking thread ID:
      m_threadID = pthread_self();
    }
    m_count++;
  }
  
  void unlock() {
    // Only the locking thread can affect anything:
    if( pthread_equal(m_threadID, pthread_self()) )
      if( m_count >= 1 ) {
	m_count--;
	// Release lock:
	if( !m_count ) {
	  m_threadID = 0;
	  pthread_mutex_unlock( &m_mutex );
	}
      }
  }

 private:
  pthread_mutex_t m_mutex;
  pthread_t m_threadID;
  int m_count;
};

class Locker {
 public:
  Locker( Mutex& mutex ) : m_mutex( mutex ) { m_mutex.lock(); }
  
  ~Locker() { m_mutex.unlock(); }
  
 private:
  Mutex& m_mutex;
};

#endif // THREAD_H
