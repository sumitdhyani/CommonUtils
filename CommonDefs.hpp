#pragma once
#include <memory>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>


//Cross compilablable for linux and windows
#ifndef __FUNCTION_NAME__
#ifdef WIN32   //WINDOWS
#define __FUNCTION_NAME__   __FUNCTION__  
#else          //*NIX
#define __FUNCTION_NAME__   __func__ 
#endif
#endif
	

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

	inline time_point now()
	{
		return std::chrono::system_clock::now();
	}

	struct NullableException : std::runtime_error
	{
		virtual operator bool() const = 0;
		NullableException(std::string msg) : std::runtime_error(msg)
		{}
	};

}