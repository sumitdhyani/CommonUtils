#pragma once
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#define define_ptr(type) typedef std::shared_ptr<type> type##_SPtr;
#define define_unique_ptr(type) typedef std::unique_ptr<type> type##_UPtr;

typedef std::mutex stdMutex;
typedef std::condition_variable stdConditionVariable;
typedef std::unique_lock<std::mutex> stdUniqueLock;
typedef std::thread stdThread;
typedef std::chrono::system_clock::time_point time_point;
typedef std::chrono::system_clock::duration duration;

define_ptr(stdMutex)
define_ptr(stdConditionVariable)
typedef unsigned int uint;


namespace ULCommonUtils
{

	time_point now();

}