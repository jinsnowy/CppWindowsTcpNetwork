#pragma once


DECLSPEC_ALIGN(16)
struct MemoryHeader : SLIST_ENTRY
{
	size_t _alloc_size;

	static MemoryHeader* attach(void* ptr)
	{
		MemoryHeader* header = static_cast<MemoryHeader*>(ptr);
		return --header;
	}

	static void* detach(MemoryHeader* header, size_t alloc_size)
	{
		header->_alloc_size = alloc_size;
		return ++header;
	}
};

DECLSPEC_ALIGN(16)
class MemoryPool
{
	enum
	{
		LARGEST_MEMORY_SIZE = 4096
	};
private:
	SLIST_HEADER _header;
	size_t		 _chunk_size;
	size_t		 _base_size;

public:
	static void initialize();

	static void* allocate(size_t size);
	static void  deallocate(void* ptr);

private:
	static MemoryPool* create(size_t base_size)
	{
		void* ptr = _aligned_malloc(sizeof(MemoryPool), 16);
		return new (ptr)MemoryPool(base_size);
	}

	MemoryPool(size_t base_size);
	~MemoryPool();

	void  Push(MemoryHeader* memory);
	MemoryHeader* Pop();

	static vector<MemoryPool*> s_memoryPoolHolder;
	static MemoryPool* s_memoryPool[LARGEST_MEMORY_SIZE + 1];
};

template<typename T, typename ...Args>
T* pool_new(Args&&... args)
{
	void* ptr = MemoryPool::allocate(sizeof(T));
	return new (ptr)T(std::forward<Args>(args)...);
}

template<typename T>
void pool_delete(T* ptr)
{
	ptr->~T();
	MemoryPool::deallocate(ptr);
}