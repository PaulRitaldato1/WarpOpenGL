#pragma once

#include <atomic>
#include <thread>

class Futex
{
public:
	void lock()
	{
		while (Flag.test_and_set(std::memory_order_acquire))
		{
			std::this_thread::yield();
		}
	}

	void unlock()
	{
		Flag.clear(std::memory_order_release);
	}
private:

	std::atomic_flag Flag = ATOMIC_FLAG_INIT;
};