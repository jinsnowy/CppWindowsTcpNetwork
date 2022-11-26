#pragma once

#define USE_PROTOCOL(p)\
template<>\
int32 Protocol::GetProtocol<p>();

#define DEFINE_PROTOCOL(p, v)\
template<>\
int32 Protocol::GetProtocol<p>() { return v; }

class Protocol
{
public:
	template<typename T>
	static int32 GetProtocol() { return 0; }
};
