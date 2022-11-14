#pragma once
#include <chrono>
#include <shared_mutex>
#include <thread>
#include <sstream>

using SharedMutex = std::shared_mutex;

struct StdThreadId
{
	static inline size_t Get()
	{
		struct ThreadIdInit
		{
			size_t Id;

			ThreadIdInit()
			{
				Id = std::hash<std::thread::id>{}(std::this_thread::get_id());
				if (Id == 0) {
					throw std::runtime_error("Thread Id cannot be zero");
				}
			}
		};

		thread_local ThreadIdInit threadId;

		return threadId.Id;
	}
};

class StdLock
{
private:
	size_t		_owner_thread;
	uint32		_write_count;
	SharedMutex _mutex;

public:
	StdLock() : _owner_thread(0), _write_count(0), _mutex{}
	{
	}

	void lock()
	{
		size_t threadId = StdThreadId::Get();

		if (_owner_thread == threadId) {
			++_write_count;
			return;
		}

		_mutex.lock();

		++_write_count;
		_owner_thread = threadId;
	}

	void unlock()
	{
		size_t threadId = StdThreadId::Get();

		if (_owner_thread != threadId) {
			std::stringstream ss;
			ss << "thread is not owned by " << threadId << endl;
			throw std::runtime_error(ss.str());
		}

		if (_write_count == 0) {
			std::stringstream ss;
			ss << "multiple unlocks by" << threadId << endl;
			throw std::runtime_error(ss.str());
		}

		--_write_count;
		if (_write_count == 0) {
			_owner_thread = 0;
			_mutex.unlock();
		}
	}

	void lock_shared() {
		size_t threadId = StdThreadId::Get();
		if (_owner_thread == threadId) {
			return;
		}

		_mutex.lock_shared();
	}

	void unlock_shared() {
		size_t threadId = StdThreadId::Get();
		if (_owner_thread == threadId) {
			return;
		}

		_mutex.unlock_shared();
	}
};

using StdWriteLock = std::unique_lock<StdLock>;
using StdReadLock = std::shared_lock<StdLock>;