#pragma once

#include "Core/Types.h"

// TODO: Replace std atomic
#include <atomic>
#include <mutex>
#include <condition_variable>
//#include <stdatomic.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#endif

#ifdef __APPLE__
// ?
#endif
#ifdef __unix__
#include <semaphore.h>
#endif


namespace Voxel {

struct Semaphore {
#ifdef _WIN32
	void* win_handle;
#elif __APPLE__
    semaphore_t apple_handle;
#else
    sem_t unix_handle;
#endif

    void init();
    void destroy();
    void post(int count = 1);
    void wait();
};

#ifndef _WIN32
struct Mutex {
    std::atomic<u32> count;
    Semaphore semaphore;

    void init();
    void destroy();
    void lock();
    void unlock();
};
#else
struct Mutex {
	std::mutex mutex;
	std::atomic<u32> count;
	Semaphore semaphore;

	void init() {}
	void destroy() {}
	void lock() { this->mutex.lock(); }
	void unlock() { this->mutex.unlock(); }
};
#endif

struct Condition_Variable {
	Semaphore semaphore;
	int count;
	int signals;
	//std::atomic<int> count;
	//std::condition_variable cv;

	void init();
	void destroy();
	void post(Mutex* mutex, int count = 1);
	void post_all(Mutex* mutex);
	void wait(Mutex* mutex);
};

struct alignas(64) Spin_Mutex {
    std::atomic<u64> count;

    void init() {}
    void destroy() {}
    void lock();
    void unlock();
};

struct RW_Mutex {
    Mutex read_mutex;
    Mutex write_mutex;
    std::atomic<u32> count;

    void init();
    void destroy();
    void read_lock();
    void read_unlock();
    void write_lock();
    void write_unlock();
};

struct Thread {
#ifdef _WIN32
    void* win_handle;
#else
    pthread_t unix_handle;
#endif
    void (*function)(void*);
    void* data;

    void init();
    void start(void (*function)(void*), void* data = NULL);
    void join();
};

u32 thread_get_id();

/*struct alignas(s32) Atomic32 {
    s32 volatile value;

    void store(s32 value);
    s32 load();
    s32 compare_exchange(s32 expected, s32 desired);
    s32 exchange(s32 value);
    s32 fetch_add(s32 value);
};

struct alignas(s64) Atomic64 {
    s64 volatile value;

    void store(s64 value);
    s64 load();
    s64 compare_exchange(s64 expected, s64 desired);
    s64 exchange(s64 value);
    s64 fetch_add(s64 value);
};

struct alignas(void*) Atomic_Ptr {
    void* volatile value;

    void store(void* value);
    void* load();
    void* compare_exchange(void* expected, void* desired);
    void* exchange(void* value);
    void* fetch_add(void* value);
};*/

} // namespace Voxel
