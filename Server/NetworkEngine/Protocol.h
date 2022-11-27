#pragma once

#define USE_PROTOCOL(p)\
template<>\
int Protocol::GetProtocol<p>();

#define DEFINE_PROTOCOL(p, v)\
template<>\
int Protocol::GetProtocol<p>() { return v; }

class Protocol
{
public:
	template<typename T>
	static int GetProtocol() { return 0; }
};
