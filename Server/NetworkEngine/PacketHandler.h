#pragma once

class PacketHandler
{
public:
	virtual bool IsValidProtocol(int32 protocol) abstract;

	virtual void HandleRecv(SessionPtrCRef session, const PacketHeader& header, CHAR* buffer) abstract;

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
	T* interpret(CHAR* buffer) { return reinterpret_cast<T*>(buffer); }

	template<typename T>
	T parse(CHAR* buffer, int32 len)
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
	static vector<PacketHandler*> s_PacketHandler;
};