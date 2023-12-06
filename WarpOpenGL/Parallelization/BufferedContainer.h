#pragma once

#include <Common/CommonTypes.h>
#include <mutex>
#include <Parallelization/Futex.h>
#include <condition_variable>

template<class TItemType>
class BufferedContainer
{
public:
	TItemType& GetBackContainer() { return BufferPool[(ActiveBufferIndex + 1) % 2]; }
	const TContainer& GetBackContainer() const { return BufferPool[(ActiveBufferIndex + 1) % 2]; }

	void AddItem(const TItemType& item)
	{
		std::unique_lock<Futex> lock(BufferLock);
		BufferPool[ActiveBufferIndex] = item;
	}

	void SwapBuffer()
	{
		std::unique_lock<Futex> lock(BufferLock);
		ActiveBufferIndex ^= 1;
	}

private:
	mutable Futex BufferLock;
	Array<TItemType, 2> BufferPool;
	int ActiveBufferIndex = 0;
};