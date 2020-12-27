#pragma once
#include <functional>
#include <unordered_map>
#include <exception>
#include <tuple>
#include <condition_variable>
#include <type_traits>
namespace ULCommonUtils
{

	template<typename... ArgTypes>
	struct Event
	{
		typedef std::function<void(ArgTypes...)> FunctionType;
		void operator +=(std::pair<size_t, FunctionType> listener)
		{
			if (m_listeners.find(listener.first) == m_listeners.end())
				m_listeners[listener.first] = listener.second;
			else
				throw std::runtime_error("ID already taken");
		}

		void operator -=(size_t id)
		{
			auto it = m_listeners.find(id);
			if (it != m_listeners.end())
				m_listeners.erase(it);
			else
				throw std::runtime_error("Invalid ID provided");
		}

		operator bool() const
		{
			return m_listeners.empty();
		}

		void operator()(ArgTypes... args)
		{
			for (auto [id, listener] : m_listeners)
				listener(args...);
		}

	private:
		std::unordered_map<size_t, FunctionType> m_listeners;
	};

	template <typename T>
	void generateHash(size_t& seed, const T& val)
	{
		seed ^= std::hash<T>()(val) + 0x9e3779b9
			+ (seed << 6) + (seed >> 2);
	}


	template <class T, class... ArgTypes>
	size_t generateHash(const T& val, const ArgTypes&... args)
	{
		size_t seed = 0;
		generateHash(seed, val);
		generateHash(seed, args...);
		return seed;
	}
}