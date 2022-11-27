#pragma once
#include <vector>
#include <memory>
#include "PacketHeader.h"

using SessionPtrCRef = const std::shared_ptr<class Session>&;
class PacketHandler
{
public:
	virtual bool IsValidProtocol(int protocol) abstract;

	virtual void HandleRecv(SessionPtrCRef session, const PacketHeader& header, char* buffer) abstract;

private:
	template<typename T>
	struct PacketInstaller
	{
		PacketInstaller()
		{
			static T handler;
			s_PacketHandler.push_back(&handler);
		}
	};

protected:
	template<typename T>
	T* interpret(char* buffer) { return reinterpret_cast<T*>(buffer); }

	template<typename T>
	T parse(char* buffer, int len)
	{ 
		T pkt;
		pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader));
		return pkt;
	}

public:
	template<typename T>
	static void Install()
	{
		static PacketInstaller<T> installer;
	}

	template<typename T>
	static PacketHandler* GetHandler()
	{
		for (auto& packetHandler : s_PacketHandler)
		{
			if (dynamic_cast<T*>(packetHandler) != nullptr)
				return packetHandler;
		}

		PacketHandler::Install<T>();

		return PacketHandler::GetHandler<T>();
	}

private:
	static std::vector<PacketHandler*> s_PacketHandler;
};