#pragma once

template<typename T>
class TlsMemory
{
public:
	static T& Get()
	{
		thread_local T inst;
		return inst;
	}
};