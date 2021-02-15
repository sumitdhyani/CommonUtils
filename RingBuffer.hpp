#pragma once
#include "CommonUtils/CommonDefs.hpp"
#include <queue>

namespace ULCommonUtils
{
	template <class T>
	class RingBuffer
	{
		std::queue<T> m_queue;
		size_t m_capacity;
	public:
		RingBuffer(size_t capacity) :
			m_capacity(capacity)
		{
		}

		void push(T item)
		{
			if (full())
				pop();

			m_queue.push(item);
		}

		void pop()
		{
			if (empty())
				throw std::runtime_error("Trying to pop empty queue");

			m_queue.pop();
		}

		size_t size()
		{
			return m_queue.size();
		}

		size_t empty()
		{
			return (size() == 0);
		}

		bool full()
		{
			return (size() == m_capacity);
		}

		T front()
		{
			if (empty())
				throw std::runtime_error("Trying to access element when buffer is empty");

			return m_queue.front();
		}

		T back()
		{
			if (empty())
				throw std::runtime_error("Trying to access element when buffer is empty");

			return m_queue.back();
		}
	};
}
