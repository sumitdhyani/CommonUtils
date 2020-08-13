#pragma once
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#define DEFINE_PTR(type) typedef std::shared_ptr<type> type##_SPtr;
#define DEFINE_UNIQUE_PTR(type) typedef std::unique_ptr<type> type##_UPtr;

typedef std::mutex stdMutex;
typedef std::condition_variable stdConditionVariable;
typedef std::unique_lock<std::mutex> stdUniqueLock;
typedef std::thread stdThread;
typedef std::chrono::system_clock::time_point time_point;
typedef std::chrono::system_clock::duration duration;

DEFINE_PTR(stdMutex)
DEFINE_PTR(stdConditionVariable)
typedef unsigned int uint;


namespace ULCommonUtils
{

	time_point now();

}