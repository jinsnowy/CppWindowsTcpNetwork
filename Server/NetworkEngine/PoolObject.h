#pragma once

class PoolObject
{
public:
	virtual ~PoolObject() = default;

	void* operator new(size_t size)
	{
		return MemoryPool::allocate(size);
	}

	void operator delete(void* ptr)
	{
		MemoryPool::deallocate(ptr);
	}
};