#pragma once
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#define define_ptr(type) typedef std::shared_ptr<type> type##_sptr;
#define define_unique_ptr(type) typedef std::unique_ptr<type> type##_uptr;

typedef std::mutex stdmutex;
typedef std::condition_variable stdconnditionvariable;
typedef std::unique_lock<std::mutex> stduniquelock;
typedef std::thread stdthread;
typedef std::chrono::system_clock::time_point time_point;
typedef std::chrono::system_clock::duration duration;

define_ptr(stdmutex)
define_ptr(stdconnditionvariable)
typedef unsigned int uint;


namespace ULCommonUtils
{

	time_point now();

}