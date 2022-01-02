/*
------------------------------------------------------------------------------
          Licensing information can be found at the end of the file.
------------------------------------------------------------------------------

yk__thread.h - v0.3 - Cross platform threading functions for C/C++.

Do this:
    #define YK__THREAD_IMPLEMENTATION
before you include this file in *one* C/C++ file to create the implementation.
*/
#ifndef yk__thread_h
#define yk__thread_h
#ifndef YK__THREAD_U64
#define YK__THREAD_U64 unsigned long long
#endif
#define YK__THREAD_STACK_SIZE_DEFAULT (0)
#define YK__THREAD_SIGNAL_WAIT_INFINITE (-1)
#define YK__THREAD_QUEUE_WAIT_INFINITE (-1)
typedef void *yk__thread_id_t;
yk__thread_id_t yk__thread_current_thread_id(void);
void yk__thread_yield(void);
void yk__thread_set_high_priority(void);
void yk__thread_exit(int return_code);
typedef void *yk__thread_ptr_t;
yk__thread_ptr_t yk__thread_create(int (*yk__thread_proc)(void *),
                                   void *user_data, char const *name,
                                   int stack_size);
void yk__thread_destroy(yk__thread_ptr_t yk__thread);
int yk__thread_join(yk__thread_ptr_t yk__thread);
typedef union yk__thread_mutex_t yk__thread_mutex_t;
void yk__thread_mutex_init(yk__thread_mutex_t *mutex);
void yk__thread_mutex_term(yk__thread_mutex_t *mutex);
void yk__thread_mutex_lock(yk__thread_mutex_t *mutex);
void yk__thread_mutex_unlock(yk__thread_mutex_t *mutex);
typedef union yk__thread_signal_t yk__thread_signal_t;
void yk__thread_signal_init(yk__thread_signal_t *signal);
void yk__thread_signal_term(yk__thread_signal_t *signal);
void yk__thread_signal_raise(yk__thread_signal_t *signal);
int yk__thread_signal_wait(yk__thread_signal_t *signal, int timeout_ms);
typedef union yk__thread_atomic_int_t yk__thread_atomic_int_t;
int yk__thread_atomic_int_load(yk__thread_atomic_int_t *atomic);
void yk__thread_atomic_int_store(yk__thread_atomic_int_t *atomic, int desired);
int yk__thread_atomic_int_inc(yk__thread_atomic_int_t *atomic);
int yk__thread_atomic_int_dec(yk__thread_atomic_int_t *atomic);
int yk__thread_atomic_int_add(yk__thread_atomic_int_t *atomic, int value);
int yk__thread_atomic_int_sub(yk__thread_atomic_int_t *atomic, int value);
int yk__thread_atomic_int_swap(yk__thread_atomic_int_t *atomic, int desired);
int yk__thread_atomic_int_compare_and_swap(yk__thread_atomic_int_t *atomic,
                                           int expected, int desired);
typedef union yk__thread_atomic_ptr_t yk__thread_atomic_ptr_t;
void *yk__thread_atomic_ptr_load(yk__thread_atomic_ptr_t *atomic);
void yk__thread_atomic_ptr_store(yk__thread_atomic_ptr_t *atomic,
                                 void *desired);
void *yk__thread_atomic_ptr_swap(yk__thread_atomic_ptr_t *atomic,
                                 void *desired);
void *yk__thread_atomic_ptr_compare_and_swap(yk__thread_atomic_ptr_t *atomic,
                                             void *expected, void *desired);
typedef union yk__thread_timer_t yk__thread_timer_t;
void yk__thread_timer_init(yk__thread_timer_t *timer);
void yk__thread_timer_term(yk__thread_timer_t *timer);
void yk__thread_timer_wait(yk__thread_timer_t *timer,
                           YK__THREAD_U64 nanoseconds);
typedef void *yk__thread_tls_t;
yk__thread_tls_t yk__thread_tls_create(void);
void yk__thread_tls_destroy(yk__thread_tls_t tls);
void yk__thread_tls_set(yk__thread_tls_t tls, void *value);
void *yk__thread_tls_get(yk__thread_tls_t tls);
typedef struct yk__thread_queue_t yk__thread_queue_t;
void yk__thread_queue_init(yk__thread_queue_t *queue, int size, void **values,
                           int count);
void yk__thread_queue_term(yk__thread_queue_t *queue);
int yk__thread_queue_produce(yk__thread_queue_t *queue, void *value,
                             int timeout_ms);
void *yk__thread_queue_consume(yk__thread_queue_t *queue, int timeout_ms);
int yk__thread_queue_count(yk__thread_queue_t *queue);
#endif /* yk__thread_h */
/**

yk__thread.h 
========

Cross platform threading functions for C/C++.

Example
-------

Here's a basic sample program which starts a second yk__thread which just waits and prints a message.

    #define  YK__THREAD_IMPLEMENTATION
    #include "yk__thread.h"

    #include <stdio.h> // for printf
    
    int yk__thread_proc( void* user_data) {
        yk__thread_timer_t timer;
        yk__thread_timer_init( &timer );

        int count = 0;
        yk__thread_atomic_int_t* exit_flag = (yk__thread_atomic_int_t*) user_data;
        while( yk__thread_atomic_int_load( exit_flag ) == 0 ) {
            printf( "Thread... " );
            yk__thread_timer_wait( &timer, 1000000000 ); // sleep for a second
            ++count;
        }

        yk__thread_timer_term( &timer );
        printf( "Done\n" );
        return count;
    }

    int main( int argc, char** argv ) {        
        yk__thread_atomic_int_t exit_flag;
        yk__thread_atomic_int_store( &exit_flag, 0 );

        yk__thread_ptr_t yk__thread = yk__thread_create( yk__thread_proc, &exit_flag, "Example yk__thread", YK__THREAD_STACK_SIZE_DEFAULT );

        yk__thread_timer_t timer;
        yk__thread_timer_init( &timer );
        for( int i = 0; i < 5; ++i ) {
            printf( "Main... " );
            yk__thread_timer_wait( &timer, 2000000000 ); // sleep for two seconds
        }
        yk__thread_timer_term( &timer );
        
        yk__thread_atomic_int_store( &exit_flag, 1 ); // signal yk__thread to exit
        int retval = yk__thread_join( yk__thread );

        printf( "Count: %d\n", retval );

        yk__thread_destroy( yk__thread );
        return retval;
    }


API Documentation
-----------------

yk__thread.h is a single-header library, and does not need any .lib files or other binaries, or any build scripts. To use it,
you just include yk__thread.h to get the API declarations. To get the definitions, you must include yk__thread.h from *one* 
single C or C++ file, and #define the symbol `YK__THREAD_IMPLEMENTATION` before you do. 


### Customization

yk__thread.h allows for specifying the exact type of 64-bit unsigned integer to be used in its API. By default, it is 
defined as `unsigned long long`, but as this is not a standard type on all compilers, you can redefine it by #defining 
YK__THREAD_U64 before including yk__thread.h. This is useful if you, for example, use the types from `<stdint.h>` in the rest of 
your program, and you want yk__thread.h to use compatible types. In this case, you would include yk__thread.h using the 
following code:

    #define YK__THREAD_U64 uint64_t
    #include "yk__thread.h"

Note that when customizing this data type, you need to use the same definition in every place where you include 
yk__thread.h, as it affect the declarations as well as the definitions.


yk__thread_current_thread_id
------------------------

    yk__thread_id_t yk__thread_current_thread_id( void )

Returns a unique identifier for the calling yk__thread. After the yk__thread terminates, the id might be reused for new threads.


yk__thread_yield
------------

    void yk__thread_yield( void )

Makes the calling yk__thread yield execution to another yk__thread. The operating system controls which yk__thread is switched to.


yk__thread_set_high_priority
------------------------

    void yk__thread_set_high_priority( void )

When created, threads are set to run at normal priority. In some rare cases, such as a sound buffer update loop, it can
be necessary to have one yk__thread of your application run on a higher priority than the rest. Calling 
`yk__thread_set_high_priority` will raise the priority of the calling yk__thread, giving it a chance to be run more often.
Do not increase the priority of a yk__thread unless you absolutely have to, as it can negatively affect performance if used
without care.


yk__thread_exit
-----------

    void yk__thread_exit( int return_code )

Exits the calling yk__thread, as if you had done `return return_code;` from the main body of the yk__thread function.


yk__thread_create
-------------

    yk__thread_ptr_t yk__thread_create( int (*yk__thread_proc)( void* ), void* user_data, char const* name, int stack_size )

Creates a new yk__thread running the `yk__thread_proc` function, passing the `user_data` through to it. The yk__thread will be 
given the debug name given in the `name` parameter, if supported on the platform, and it will have the stack size
specified in the `stack_size` parameter. To get the operating system default stack size, use the defined constant
`YK__THREAD_STACK_SIZE_DEFAULT`. When returning from the yk__thread_proc function, the value you return can be received in
another yk__thread by calling yk__thread_join. `yk__thread_create` returns a pointer to the yk__thread instance, which can be used 
as a parameter to the functions `yk__thread_destroy` and `yk__thread_join`.


yk__thread_destroy
--------------

    void yk__thread_destroy( yk__thread_ptr_t yk__thread )

Destroys a yk__thread that was created by calling `yk__thread_create`. Make sure the yk__thread has exited before you attempt to 
destroy it. This can be accomplished by calling `yk__thread_join`. It is not possible for force termination of a yk__thread by
calling `yk__thread_destroy`.


yk__thread_join
-----------

    int yk__thread_join( yk__thread_ptr_t yk__thread )

Waits for the specified yk__thread to exit. Returns the value which the yk__thread returned when exiting.


yk__thread_mutex_init
-----------------
    
    void yk__thread_mutex_init( yk__thread_mutex_t* mutex )

Initializes the specified mutex instance, preparing it for use. A mutex can be used to lock sections of code, such that
it can only be run by one yk__thread at a time.


yk__thread_mutex_term
-----------------

    void yk__thread_mutex_term( yk__thread_mutex_t* mutex )

Terminates the specified mutex instance, releasing any system resources held by it.


yk__thread_mutex_lock
-----------------

    void yk__thread_mutex_lock( yk__thread_mutex_t* mutex )

Takes an exclusive lock on a mutex. If the lock is already taken by another yk__thread, `yk__thread_mutex_lock` will yield the
calling yk__thread and wait for the lock to become available before returning. The mutex must be initialized by calling
`yk__thread_mutex_init` before it can be locked.


yk__thread_mutex_unlock
-------------------

    void yk__thread_mutex_unlock( yk__thread_mutex_t* mutex )

Releases a lock taken by calling `yk__thread_mutex_lock`. 


yk__thread_signal_init
------------------

    void yk__thread_signal_init( yk__thread_signal_t* signal )

Initializes the specified signal instance, preparing it for use. A signal works like a flag, which can be waited on by
one yk__thread, until it is raised from another yk__thread. 


yk__thread_signal_term
------------------

    void yk__thread_signal_term( yk__thread_signal_t* signal )

Terminates the specified signal instance, releasing any system resources held by it.


yk__thread_signal_raise
-------------------

    void yk__thread_signal_raise( yk__thread_signal_t* signal )

Raise the specified signal. Other threads waiting for the signal will proceed.  


yk__thread_signal_wait
------------------

    int yk__thread_signal_wait( yk__thread_signal_t* signal, int timeout_ms )

Waits for a signal to be raised, or until `timeout_ms` milliseconds have passed. If the wait timed out, a value of 0 is
returned, otherwise a non-zero value is returned. If the `timeout_ms` parameter is YK__THREAD_SIGNAL_WAIT_INFINITE, 
`yk__thread_signal_wait` waits indefinitely.


yk__thread_atomic_int_load
----------------------

    int yk__thread_atomic_int_load( yk__thread_atomic_int_t* atomic )

Returns the value of `atomic` as an atomic operation.


yk__thread_atomic_int_store
-----------------------

    void yk__thread_atomic_int_store( yk__thread_atomic_int_t* atomic, int desired )

Sets the value of `atomic` as an atomic operation.


yk__thread_atomic_int_inc
---------------------

    int yk__thread_atomic_int_inc( yk__thread_atomic_int_t* atomic )

Increments the value of `atomic` by one, as an atomic operation. Returns the value `atomic` had before the operation.


yk__thread_atomic_int_dec
---------------------

    int yk__thread_atomic_int_dec( yk__thread_atomic_int_t* atomic )

Decrements the value of `atomic` by one, as an atomic operation. Returns the value `atomic` had before the operation.


yk__thread_atomic_int_add
---------------------

    int yk__thread_atomic_int_add( yk__thread_atomic_int_t* atomic, int value )

Adds the specified value to `atomic`, as an atomic operation. Returns the value `atomic` had before the operation.


yk__thread_atomic_int_sub
---------------------

    int yk__thread_atomic_int_sub( yk__thread_atomic_int_t* atomic, int value )

Subtracts the specified value to `atomic`, as an atomic operation. Returns the value `atomic` had before the operation.


yk__thread_atomic_int_swap
----------------------

    int yk__thread_atomic_int_swap( yk__thread_atomic_int_t* atomic, int desired )

Sets the value of `atomic` as an atomic operation. Returns the value `atomic` had before the operation.


yk__thread_atomic_int_compare_and_swap
----------------------------------

    int yk__thread_atomic_int_compare_and_swap( yk__thread_atomic_int_t* atomic, int expected, int desired )

Compares the value of `atomic` to the value of `expected`, and if they match, sets the vale of `atomic` to `desired`,
all as an atomic operation. Returns the value `atomic` had before the operation.


yk__thread_atomic_ptr_load
----------------------

    void* yk__thread_atomic_ptr_load( yk__thread_atomic_ptr_t* atomic )

Returns the value of `atomic` as an atomic operation.


yk__thread_atomic_ptr_store
-----------------------

    void yk__thread_atomic_ptr_store( yk__thread_atomic_ptr_t* atomic, void* desired )

Sets the value of `atomic` as an atomic operation.


yk__thread_atomic_ptr_swap
----------------------

    void* yk__thread_atomic_ptr_swap( yk__thread_atomic_ptr_t* atomic, void* desired )

Sets the value of `atomic` as an atomic operation. Returns the value `atomic` had before the operation.


yk__thread_atomic_ptr_compare_and_swap
----------------------------------

    void* yk__thread_atomic_ptr_compare_and_swap( yk__thread_atomic_ptr_t* atomic, void* expected, void* desired )

Compares the value of `atomic` to the value of `expected`, and if they match, sets the vale of `atomic` to `desired`,
all as an atomic operation. Returns the value `atomic` had before the operation.


yk__thread_timer_init
-----------------
    
    void yk__thread_timer_init( yk__thread_timer_t* timer )

Initializes the specified timer instance, preparing it for use. A timer can be used to sleep a yk__thread for a high 
precision duration.


yk__thread_timer_term
-----------------

    void yk__thread_timer_term( yk__thread_timer_t* timer )

Terminates the specified timer instance, releasing any system resources held by it.


yk__thread_timer_wait
-----------------

    void yk__thread_timer_wait( yk__thread_timer_t* timer, YK__THREAD_U64 nanoseconds )

Waits until `nanoseconds` amount of time have passed, before returning.


yk__thread_tls_create
-----------------
    
    yk__thread_tls_t yk__thread_tls_create( void )

Creates  a yk__thread local storage (TLS) index. Once created, each yk__thread has its own value for that TLS index, which can
be set or retrieved individually.


yk__thread_tls_destroy
------------------
    
    void yk__thread_tls_destroy( yk__thread_tls_t tls )

Destroys the specified TLS index. No further calls to `yk__thread_tls_set` or `yk__thread_tls_get` are valid after this.


yk__thread_tls_set
--------------

    void yk__thread_tls_set( yk__thread_tls_t tls, void* value )

Stores a value in the calling yk__thread's slot for the specified TLS index. Each yk__thread has its own value for each TLS 
index.


yk__thread_tls_get
--------------

    void* yk__thread_tls_get( yk__thread_tls_t tls )

Retrieves the value from the calling yk__thread's slot for the specified TLS index. Each yk__thread has its own value for each 
TLS index.


yk__thread_queue_init
-----------------

    void yk__thread_queue_init( yk__thread_queue_t* queue, int size, void** values, int count )

Initializes the specified queue instance, preparing it for use. The queue is a lock-free (but not wait-free)
single-producer/single-consumer queue - it will not acquire any locks as long as there is space for adding or items to
be consume, but will lock and wait when there is not. The `size` parameter specifies the number of elements in the 
queue. The `values` parameter is an array of queue slots (`size` elements in length), each being of type `void*`. If
the queue is initially empty, the `count` parameter should be 0, otherwise it indicates the number of entires, from the
start of the `values` array, that the queue is initialized with. The `values` array is not copied, and must remain valid
until `yk__thread_queue_term` is called.


yk__thread_queue_term
-----------------

    void yk__thread_queue_term( yk__thread_queue_t* queue )

Terminates the specified queue instance, releasing any system resources held by it.


yk__thread_queue_produce
--------------------

    int yk__thread_queue_produce( yk__thread_queue_t* queue, void* value, int timeout_ms )

Adds an element to a single-producer/single-consumer queue. If there is space in the queue to add another element, no
lock will be taken. If the queue is full, calling yk__thread will sleep until an element is consumed from another yk__thread, 
before adding the element, or until `timeout_ms` milliseconds have passed. If the wait timed out, a value of 0 is 
returned, otherwise a non-zero value is returned. If the `timeout_ms` parameter is YK__THREAD_QUEUE_WAIT_INFINITE,  
`yk__thread_queue_produce` waits indefinitely.


yk__thread_queue_consume
--------------------

    void* yk__thread_queue_consume( yk__thread_queue_t* queue, int timeout_ms )

Removes an element from a single-producer/single-consumer queue. If the queue contains at least one element, no lock 
will be taken. If the queue is empty, the calling yk__thread will sleep until an element is added from another yk__thread, or 
until `timeout_ms` milliseconds have passed. If the wait timed out, a value of NULL is returned, otherwise 
`yk__thread_queue_consume` returns the value that was removed from the queue. If the `timeout_ms` parameter is 
YK__THREAD_QUEUE_WAIT_INFINITE, `yk__thread_queue_consume` waits indefinitely.


yk__thread_queue_count
------------------

    int yk__thread_queue_count( yk__thread_queue_t* queue )

Returns the number of elements currently held in a single-producer/single-consumer queue. Be aware that by the time you
get the count, it might have changed by another yk__thread calling consume or produce, so use with care.

*/
/*
----------------------
    IMPLEMENTATION
----------------------
*/
#ifndef yk__thread_impl
#define yk__thread_impl
union yk__thread_mutex_t {
  void *align;
  char data[64];
};
union yk__thread_signal_t {
  void *align;
  char data[116];
};
union yk__thread_atomic_int_t {
  void *align;
  long i;
};
union yk__thread_atomic_ptr_t {
  void *ptr;
};
union yk__thread_timer_t {
  void *data;
  char d[8];
};
struct yk__thread_queue_t {
  yk__thread_signal_t data_ready;
  yk__thread_signal_t space_open;
  yk__thread_atomic_int_t count;
  yk__thread_atomic_int_t head;
  yk__thread_atomic_int_t tail;
  void **values;
  int size;
#ifndef NDEBUG
  yk__thread_atomic_int_t id_produce_is_set;
  yk__thread_id_t id_produce;
  yk__thread_atomic_int_t id_consume_is_set;
  yk__thread_id_t id_consume;
#endif
};
#endif /* yk__thread_impl */
#ifdef YK__THREAD_IMPLEMENTATION
#undef YK__THREAD_IMPLEMENTATION
#if defined(_WIN32)
#pragma comment(lib, "winmm.lib")
#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#if !defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0501
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x501// requires Windows XP minimum
#endif
#define _WINSOCKAPI_
#pragma warning(push)
#pragma warning(                                                               \
    disable : 4668)// 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
#pragma warning(disable : 4255)
#include <windows.h>
#pragma warning(pop)
// To set yk__thread name
const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO {
  DWORD dwType;
  LPCSTR szName;
  DWORD dwThreadID;
  DWORD dwFlags;
} YK__THREADNAME_INFO;
#pragma pack(pop)
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
#include <pthread.h>
#include <sys/time.h>
#else
#error Unknown platform.
#endif
#ifndef NDEBUG
#include <assert.h>
#endif
yk__thread_id_t yk__thread_current_thread_id(void) {
#if defined(_WIN32)
  return (void *) (uintptr_t) GetCurrentThreadId();
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  return (void *) pthread_self();
#else
#error Unknown platform.
#endif
}
void yk__thread_yield(void) {
#if defined(_WIN32)
  SwitchToThread();
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  sched_yield();
#else
#error Unknown platform.
#endif
}
void yk__thread_exit(int return_code) {
#if defined(_WIN32)
  ExitThread((DWORD) return_code);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  pthread_exit((void *) (uintptr_t) return_code);
#else
#error Unknown platform.
#endif
}
yk__thread_ptr_t yk__thread_create(int (*yk__thread_proc)(void *),
                                   void *user_data, char const *name,
                                   int stack_size) {
#if defined(_WIN32)
  DWORD yk__thread_id;
  HANDLE handle =
      CreateThread(NULL, stack_size > 0 ? (size_t) stack_size : 0U,
                   (LPTHREAD_START_ROUTINE) (uintptr_t) yk__thread_proc,
                   user_data, 0, &yk__thread_id);
  if (!handle) return NULL;
  // Yes, this crazy construct with __try and RaiseException is how you name a yk__thread in Visual Studio :S
  if (name && IsDebuggerPresent()) {
    YK__THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = name;
    info.dwThreadID = yk__thread_id;
    info.dwFlags = 0;
    __try {
      RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR),
                     (ULONG_PTR *) &info);
    } __except (EXCEPTION_EXECUTE_HANDLER) {}
  }
  return (yk__thread_ptr_t) handle;
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  pthread_t yk__thread;
  if (0 != pthread_create(&yk__thread, NULL,
                          (void *(*) (void *) ) yk__thread_proc, user_data))
    return NULL;
#if !defined(__APPLE__)// max doesn't support pthread_setname_np. alternatives?
  if (name) pthread_setname_np(yk__thread, name);
#endif
  return (yk__thread_ptr_t) yk__thread;
#else
#error Unknown platform.
#endif
}
void yk__thread_destroy(yk__thread_ptr_t yk__thread) {
#if defined(_WIN32)
  WaitForSingleObject((HANDLE) yk__thread, INFINITE);
  CloseHandle((HANDLE) yk__thread);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  pthread_join((pthread_t) yk__thread, NULL);
#else
#error Unknown platform.
#endif
}
int yk__thread_join(yk__thread_ptr_t yk__thread) {
#if defined(_WIN32)
  WaitForSingleObject((HANDLE) yk__thread, INFINITE);
  DWORD retval;
  GetExitCodeThread((HANDLE) yk__thread, &retval);
  return (int) retval;
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  void *retval;
  pthread_join((pthread_t) yk__thread, &retval);
  return (int) (uintptr_t) retval;
#else
#error Unknown platform.
#endif
}
void yk__thread_set_high_priority(void) {
#if defined(_WIN32)
  SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  struct sched_param sp;
  memset(&sp, 0, sizeof(sp));
  sp.sched_priority = sched_get_priority_min(SCHED_RR);
  pthread_setschedparam(pthread_self(), SCHED_RR, &sp);
#else
#error Unknown platform.
#endif
}
void yk__thread_mutex_init(yk__thread_mutex_t *mutex) {
#if defined(_WIN32)
// Compile-time size check
#pragma warning(push)
#pragma warning(                                                               \
    disable : 4214)// nonstandard extension used: bit field types other than int
  struct x {
    char yk__thread_mutex_type_too_small
        : (sizeof(yk__thread_mutex_t) < sizeof(CRITICAL_SECTION) ? 0 : 1);
  };
#pragma warning(pop)
  InitializeCriticalSectionAndSpinCount((CRITICAL_SECTION *) mutex, 32);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  // Compile-time size check
  struct x {
    char yk__thread_mutex_type_too_small
        : (sizeof(yk__thread_mutex_t) < sizeof(pthread_mutex_t) ? 0 : 1);
  };
  pthread_mutex_init((pthread_mutex_t *) mutex, NULL);
#else
#error Unknown platform.
#endif
}
void yk__thread_mutex_term(yk__thread_mutex_t *mutex) {
#if defined(_WIN32)
  DeleteCriticalSection((CRITICAL_SECTION *) mutex);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  pthread_mutex_destroy((pthread_mutex_t *) mutex);
#else
#error Unknown platform.
#endif
}
void yk__thread_mutex_lock(yk__thread_mutex_t *mutex) {
#if defined(_WIN32)
  EnterCriticalSection((CRITICAL_SECTION *) mutex);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  pthread_mutex_lock((pthread_mutex_t *) mutex);
#else
#error Unknown platform.
#endif
}
void yk__thread_mutex_unlock(yk__thread_mutex_t *mutex) {
#if defined(_WIN32)
  LeaveCriticalSection((CRITICAL_SECTION *) mutex);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  pthread_mutex_unlock((pthread_mutex_t *) mutex);
#else
#error Unknown platform.
#endif
}
struct yk__thread_internal_signal_t {
#if defined(_WIN32)
#if _WIN32_WINNT >= 0x0600
  CRITICAL_SECTION mutex;
  CONDITION_VARIABLE condition;
  int value;
#else
  HANDLE event;
#endif
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  pthread_mutex_t mutex;
  pthread_cond_t condition;
  int value;
#else
#error Unknown platform.
#endif
};
void yk__thread_signal_init(yk__thread_signal_t *signal) {
// Compile-time size check
#pragma warning(push)
#pragma warning(                                                               \
    disable : 4214)// nonstandard extension used: bit field types other than int
  struct x {
    char yk__thread_signal_type_too_small
        : (sizeof(yk__thread_signal_t) <
                   sizeof(struct yk__thread_internal_signal_t)
               ? 0
               : 1);
  };
#pragma warning(pop)
  struct yk__thread_internal_signal_t *internal =
      (struct yk__thread_internal_signal_t *) signal;
#if defined(_WIN32)
#if _WIN32_WINNT >= 0x0600
  InitializeCriticalSectionAndSpinCount(&internal->mutex, 32);
  InitializeConditionVariable(&internal->condition);
  internal->value = 0;
#else
  internal->event = CreateEvent(NULL, FALSE, FALSE, NULL);
#endif
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  pthread_mutex_init(&internal->mutex, NULL);
  pthread_cond_init(&internal->condition, NULL);
  internal->value = 0;
#else
#error Unknown platform.
#endif
}
void yk__thread_signal_term(yk__thread_signal_t *signal) {
  struct yk__thread_internal_signal_t *internal =
      (struct yk__thread_internal_signal_t *) signal;
#if defined(_WIN32)
#if _WIN32_WINNT >= 0x0600
  DeleteCriticalSection(&internal->mutex);
#else
  CloseHandle(internal->event);
#endif
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  pthread_mutex_destroy(&internal->mutex);
  pthread_cond_destroy(&internal->condition);
#else
#error Unknown platform.
#endif
}
void yk__thread_signal_raise(yk__thread_signal_t *signal) {
  struct yk__thread_internal_signal_t *internal =
      (struct yk__thread_internal_signal_t *) signal;
#if defined(_WIN32)
#if _WIN32_WINNT >= 0x0600
  EnterCriticalSection(&internal->mutex);
  internal->value = 1;
  LeaveCriticalSection(&internal->mutex);
  WakeConditionVariable(&internal->condition);
#else
  SetEvent(internal->event);
#endif
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  pthread_mutex_lock(&internal->mutex);
  internal->value = 1;
  pthread_mutex_unlock(&internal->mutex);
  pthread_cond_signal(&internal->condition);
#else
#error Unknown platform.
#endif
}
int yk__thread_signal_wait(yk__thread_signal_t *signal, int timeout_ms) {
  struct yk__thread_internal_signal_t *internal =
      (struct yk__thread_internal_signal_t *) signal;
#if defined(_WIN32)
#if _WIN32_WINNT >= 0x0600
  int timed_out = 0;
  EnterCriticalSection(&internal->mutex);
  while (internal->value == 0) {
    int res = SleepConditionVariableCS(&internal->condition, &internal->mutex,
                                       timeout_ms < 0 ? INFINITE : timeout_ms);
    if (!res && GetLastError() == ERROR_TIMEOUT) {
      timed_out = 1;
      break;
    }
  }
  if (!timed_out) internal->value = 0;
  LeaveCriticalSection(&internal->mutex);
  return !timed_out;
#else
  int failed = WAIT_OBJECT_0 !=
               WaitForSingleObject(internal->event,
                                   timeout_ms < 0 ? INFINITE : timeout_ms);
  return !failed;
#endif
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  struct timespec ts;
  if (timeout_ms >= 0) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    ts.tv_sec = time(NULL) + timeout_ms / 1000;
    ts.tv_nsec = tv.tv_usec * 1000 + 1000 * 1000 * (timeout_ms % 1000);
    ts.tv_sec += ts.tv_nsec / (1000 * 1000 * 1000);
    ts.tv_nsec %= (1000 * 1000 * 1000);
  }
  int timed_out = 0;
  pthread_mutex_lock(&internal->mutex);
  while (internal->value == 0) {
    if (timeout_ms < 0)
      pthread_cond_wait(&internal->condition, &internal->mutex);
    else if (pthread_cond_timedwait(&internal->condition, &internal->mutex,
                                    &ts) == ETIMEDOUT) {
      timed_out = 1;
      break;
    }
  }
  if (!timed_out) internal->value = 0;
  pthread_mutex_unlock(&internal->mutex);
  return !timed_out;
#else
#error Unknown platform.
#endif
}
int yk__thread_atomic_int_load(yk__thread_atomic_int_t *atomic) {
#if defined(_WIN32)
  return InterlockedCompareExchange(&atomic->i, 0, 0);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  return (int) __sync_fetch_and_add(&atomic->i, 0);
#else
#error Unknown platform.
#endif
}
void yk__thread_atomic_int_store(yk__thread_atomic_int_t *atomic, int desired) {
#if defined(_WIN32)
  InterlockedExchange(&atomic->i, desired);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  __sync_lock_test_and_set(&atomic->i, desired);
  __sync_lock_release(&atomic->i);
#else
#error Unknown platform.
#endif
}
int yk__thread_atomic_int_inc(yk__thread_atomic_int_t *atomic) {
#if defined(_WIN32)
  return InterlockedIncrement(&atomic->i) - 1;
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  return (int) __sync_fetch_and_add(&atomic->i, 1);
#else
#error Unknown platform.
#endif
}
int yk__thread_atomic_int_dec(yk__thread_atomic_int_t *atomic) {
#if defined(_WIN32)
  return InterlockedDecrement(&atomic->i) + 1;
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  return (int) __sync_fetch_and_sub(&atomic->i, 1);
#else
#error Unknown platform.
#endif
}
int yk__thread_atomic_int_add(yk__thread_atomic_int_t *atomic, int value) {
#if defined(_WIN32)
  return InterlockedExchangeAdd(&atomic->i, value);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  return (int) __sync_fetch_and_add(&atomic->i, value);
#else
#error Unknown platform.
#endif
}
int yk__thread_atomic_int_sub(yk__thread_atomic_int_t *atomic, int value) {
#if defined(_WIN32)
  return InterlockedExchangeAdd(&atomic->i, -value);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  return (int) __sync_fetch_and_sub(&atomic->i, value);
#else
#error Unknown platform.
#endif
}
int yk__thread_atomic_int_swap(yk__thread_atomic_int_t *atomic, int desired) {
#if defined(_WIN32)
  return InterlockedExchange(&atomic->i, desired);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  int old = (int) __sync_lock_test_and_set(&atomic->i, desired);
  __sync_lock_release(&atomic->i);
  return old;
#else
#error Unknown platform.
#endif
}
int yk__thread_atomic_int_compare_and_swap(yk__thread_atomic_int_t *atomic,
                                           int expected, int desired) {
#if defined(_WIN32)
  return InterlockedCompareExchange(&atomic->i, desired, expected);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  return (int) __sync_val_compare_and_swap(&atomic->i, expected, desired);
#else
#error Unknown platform.
#endif
}
void *yk__thread_atomic_ptr_load(yk__thread_atomic_ptr_t *atomic) {
#if defined(_WIN32)
  return InterlockedCompareExchangePointer(&atomic->ptr, 0, 0);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  return __sync_fetch_and_add(&atomic->ptr, 0);
#else
#error Unknown platform.
#endif
}
void yk__thread_atomic_ptr_store(yk__thread_atomic_ptr_t *atomic,
                                 void *desired) {
#if defined(_WIN32)
#pragma warning(push)
#pragma warning(                                                               \
    disable : 4302)// 'type cast' : truncation from 'void *' to 'LONG'
#pragma warning(disable : 4311)// pointer truncation from 'void *' to 'LONG'
#pragma warning(                                                               \
    disable : 4312)// conversion from 'LONG' to 'PVOID' of greater size
  InterlockedExchangePointer(&atomic->ptr, desired);
#pragma warning(pop)
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  __sync_lock_test_and_set(&atomic->ptr, desired);
  __sync_lock_release(&atomic->ptr);
#else
#error Unknown platform.
#endif
}
void *yk__thread_atomic_ptr_swap(yk__thread_atomic_ptr_t *atomic,
                                 void *desired) {
#if defined(_WIN32)
#pragma warning(push)
#pragma warning(                                                               \
    disable : 4302)// 'type cast' : truncation from 'void *' to 'LONG'
#pragma warning(disable : 4311)// pointer truncation from 'void *' to 'LONG'
#pragma warning(                                                               \
    disable : 4312)// conversion from 'LONG' to 'PVOID' of greater size
  return InterlockedExchangePointer(&atomic->ptr, desired);
#pragma warning(pop)
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  void *old = __sync_lock_test_and_set(&atomic->ptr, desired);
  __sync_lock_release(&atomic->ptr);
  return old;
#else
#error Unknown platform.
#endif
}
void *yk__thread_atomic_ptr_compare_and_swap(yk__thread_atomic_ptr_t *atomic,
                                             void *expected, void *desired) {
#if defined(_WIN32)
  return InterlockedCompareExchangePointer(&atomic->ptr, desired, expected);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  return __sync_val_compare_and_swap(&atomic->ptr, expected, desired);
#else
#error Unknown platform.
#endif
}
void yk__thread_timer_init(yk__thread_timer_t *timer) {
#if defined(_WIN32)
// Compile-time size check
#pragma warning(push)
#pragma warning(                                                               \
    disable : 4214)// nonstandard extension used: bit field types other than int
  struct x {
    char yk__thread_timer_type_too_small
        : (sizeof(yk__thread_mutex_t) < sizeof(HANDLE) ? 0 : 1);
  };
#pragma warning(pop)
  TIMECAPS tc;
  if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) == TIMERR_NOERROR)
    timeBeginPeriod(tc.wPeriodMin);
  *(HANDLE *) timer = CreateWaitableTimer(NULL, TRUE, NULL);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  // Nothing
#else
#error Unknown platform.
#endif
}
void yk__thread_timer_term(yk__thread_timer_t *timer) {
#if defined(_WIN32)
  CloseHandle(*(HANDLE *) timer);
  TIMECAPS tc;
  if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) == TIMERR_NOERROR)
    timeEndPeriod(tc.wPeriodMin);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  // Nothing
#else
#error Unknown platform.
#endif
}
void yk__thread_timer_wait(yk__thread_timer_t *timer,
                           YK__THREAD_U64 nanoseconds) {
#if defined(_WIN32)
  LARGE_INTEGER due_time;
  due_time.QuadPart = -(LONGLONG) (nanoseconds / 100);
  BOOL b = SetWaitableTimer(*(HANDLE *) timer, &due_time, 0, 0, 0, FALSE);
  (void) b;
  WaitForSingleObject(*(HANDLE *) timer, INFINITE);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  struct timespec rem;
  struct timespec req;
  req.tv_sec = nanoseconds / 1000000000ULL;
  req.tv_nsec = nanoseconds - req.tv_sec * 1000000000ULL;
  while (nanosleep(&req, &rem)) req = rem;
#else
#error Unknown platform.
#endif
}
yk__thread_tls_t yk__thread_tls_create(void) {
#if defined(_WIN32)
  DWORD tls = TlsAlloc();
  if (tls == TLS_OUT_OF_INDEXES) return NULL;
  else
    return (yk__thread_tls_t) (uintptr_t) tls;
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  pthread_key_t tls;
  if (pthread_key_create(&tls, NULL) == 0) return (yk__thread_tls_t) tls;
  else
    return NULL;
#else
#error Unknown platform.
#endif
}
void yk__thread_tls_destroy(yk__thread_tls_t tls) {
#if defined(_WIN32)
  TlsFree((DWORD) (uintptr_t) tls);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  pthread_key_delete((pthread_key_t) tls);
#else
#error Unknown platform.
#endif
}
void yk__thread_tls_set(yk__thread_tls_t tls, void *value) {
#if defined(_WIN32)
  TlsSetValue((DWORD) (uintptr_t) tls, value);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  pthread_setspecific((pthread_key_t) tls, value);
#else
#error Unknown platform.
#endif
}
void *yk__thread_tls_get(yk__thread_tls_t tls) {
#if defined(_WIN32)
  return TlsGetValue((DWORD) (uintptr_t) tls);
#elif defined(__linux__) || defined(__APPLE__) || defined(__ANDROID__)
  return pthread_getspecific((pthread_key_t) tls);
#else
#error Unknown platform.
#endif
}
void yk__thread_queue_init(yk__thread_queue_t *queue, int size, void **values,
                           int count) {
  queue->values = values;
  yk__thread_signal_init(&queue->data_ready);
  yk__thread_signal_init(&queue->space_open);
  yk__thread_atomic_int_store(&queue->head, 0);
  yk__thread_atomic_int_store(&queue->tail, count > size ? size : count);
  yk__thread_atomic_int_store(&queue->count, count > size ? size : count);
  queue->size = size;
#ifndef NDEBUG
  yk__thread_atomic_int_store(&queue->id_produce_is_set, 0);
  yk__thread_atomic_int_store(&queue->id_consume_is_set, 0);
#endif
}
void yk__thread_queue_term(yk__thread_queue_t *queue) {
  yk__thread_signal_term(&queue->space_open);
  yk__thread_signal_term(&queue->data_ready);
}
int yk__thread_queue_produce(yk__thread_queue_t *queue, void *value,
                             int timeout_ms) {
#ifndef NDEBUG
  if (yk__thread_atomic_int_compare_and_swap(&queue->id_produce_is_set, 0, 1) ==
      0)
    queue->id_produce = yk__thread_current_thread_id();
  assert(yk__thread_current_thread_id() == queue->id_produce);
#endif
  if (yk__thread_atomic_int_load(&queue->count) == queue->size) {
    if (timeout_ms == 0) return 0;
    yk__thread_signal_wait(&queue->space_open,
                           timeout_ms == YK__THREAD_QUEUE_WAIT_INFINITE
                               ? YK__THREAD_SIGNAL_WAIT_INFINITE
                               : timeout_ms);
  }
  int tail = yk__thread_atomic_int_inc(&queue->tail);
  queue->values[tail % queue->size] = value;
  if (yk__thread_atomic_int_inc(&queue->count) == 0)
    yk__thread_signal_raise(&queue->data_ready);
  return 0;
}
void *yk__thread_queue_consume(yk__thread_queue_t *queue, int timeout_ms) {
#ifndef NDEBUG
  if (yk__thread_atomic_int_compare_and_swap(&queue->id_consume_is_set, 0, 1) ==
      0)
    queue->id_consume = yk__thread_current_thread_id();
  assert(yk__thread_current_thread_id() == queue->id_consume);
#endif
  if (yk__thread_atomic_int_load(&queue->count) == 0) {
    if (timeout_ms == 0) return NULL;
    yk__thread_signal_wait(&queue->data_ready, YK__THREAD_SIGNAL_WAIT_INFINITE);
  }
  int head = yk__thread_atomic_int_inc(&queue->head);
  void *retval = queue->values[head % queue->size];
  if (yk__thread_atomic_int_dec(&queue->count) == queue->size)
    yk__thread_signal_raise(&queue->space_open);
  return retval;
}
int yk__thread_queue_count(yk__thread_queue_t *queue) {
  return yk__thread_atomic_int_load(&queue->count);
}
#endif /* YK__THREAD_IMPLEMENTATION */
       /*
revision history:
    0.3     set_high_priority API change. Fixed spurious wakeup bug in signal. Added 
            timeout param to queue produce/consume. Various cleanup and trivial fixes.
    0.2     first publicly released version 
*/
       /*
------------------------------------------------------------------------------

This software is available under 2 licenses - you may choose the one you like.

------------------------------------------------------------------------------

ALTERNATIVE A - MIT License

Copyright (c) 2015 Mattias Gustavsson

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.

------------------------------------------------------------------------------

ALTERNATIVE B - Public Domain (www.unlicense.org)

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this 
software, either in source code form or as a compiled binary, for any purpose, 
commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this 
software dedicate any and all copyright interest in the software to the public 
domain. We make this dedication for the benefit of the public at large and to 
the detriment of our heirs and successors. We intend this dedication to be an 
overt act of relinquishment in perpetuity of all present and future rights to 
this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

------------------------------------------------------------------------------
*/