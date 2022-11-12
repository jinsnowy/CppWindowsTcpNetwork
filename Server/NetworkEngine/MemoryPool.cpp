#include "pch.h"
#include "MemoryPool.h"

vector<MemoryPool*> MemoryPool::s_memoryPoolHolder = {};
MemoryPool* MemoryPool::s_memoryPool[MemoryPool::LARGEST_MEMORY_SIZE + 1] = {};

void MemoryPool::initialize()
{
	struct MemoryPoolInit
	{
		MemoryPoolInit()
		{
			int idx = 0;
			const auto assign = [&idx](size_t st, size_t ed, size_t t)
			{
				for (size_t size = st; size <= ed; size += t)
				{
					MemoryPool* memPool = MemoryPool::create(size);
					do {
						s_memoryPool[idx] = memPool;
					} while (++idx <= size);

					s_memoryPoolHolder.push_back(memPool);
				}
			};

			assign(32, 64, 16);
			assign(64, 256, 64); 
			assign(256, 512, 128);
			assign(512, 2048, 256);
			assign(2048, 4096, 512);
		}

		~MemoryPoolInit()
		{
			for (auto& memPool : s_memoryPoolHolder)
			{
				memPool->~MemoryPool();
				::_aligned_free(memPool);
			}
			s_memoryPoolHolder.clear();
		}
	};

	static MemoryPoolInit mem_pool_init;
}

void* MemoryPool::allocate(size_t size)
{
	MemoryHeader* header = nullptr;
	if (size > LARGEST_MEMORY_SIZE)
	{
		header = (MemoryHeader*)::malloc(size + sizeof(MemoryHeader));
		header->_alloc_size = size;
	}
	else
	{
		header = s_memoryPool[size]->Pop();
	}

	return MemoryHeader::detach(header, size);
}

void MemoryPool::deallocate(void* ptr)
{
	MemoryHeader* header = MemoryHeader::attach(ptr);

	if (header->_alloc_size > LARGEST_MEMORY_SIZE)
	{
		::free(header);

		return;
	}

	s_memoryPool[header->_alloc_size]->Push(header);
}

MemoryPool::MemoryPool(size_t base_size)
	:
	_base_size(base_size),
	_chunk_size(base_size + sizeof(MemoryHeader))
{
	InitializeSListHead(&_header);
}

MemoryPool::~MemoryPool()
{
	while (MemoryHeader* ptr = (MemoryHeader*)InterlockedPopEntrySList(&_header))
	{
		::_aligned_free(ptr);
	}
}

void MemoryPool::Push(MemoryHeader *memory)
{
	memory->_alloc_size = 0;

	InterlockedPushEntrySList(&_header, reinterpret_cast<PSLIST_ENTRY>(memory));
}

MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* memory = reinterpret_cast<MemoryHeader*>(InterlockedPopEntrySList(&_header));
	if (memory == nullptr)
	{
		memory = (MemoryHeader*)::_aligned_malloc(_chunk_size, 16);
	}
	else
	{
		ASSERT_CRASH(memory->_alloc_size == 0);
	}

	return memory;
}
