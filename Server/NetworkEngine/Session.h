#pragma once

class Session : public std::enable_shared_from_this<Session>
{
	friend class TcpNetwork;
	friend class Listener;
	friend class HandshakePacketHandler;
public:
	Session();
	virtual ~Session();

public:
	void SendAsync(const BufferSegment& segment);

	template<typename T>
	shared_ptr<T> GetShared() { return static_pointer_cast<T>(shared_from_this()); }

	bool IsConnected();

	void Disconnect();

	void DisconnectAsync();

protected:
	shared_ptr<TcpNetwork> _network;

	virtual void onAuthorized();

	virtual void onConnected();
	
	virtual void onDisconnected();

public:
	EndPoint	 GetEndPoint();
	string		 GetEndPointDesc() { return GetEndPoint().ToString(); }
	NetworkPtr	 GetNetwork() { return _network; }

	SessionID	 GetSessionId() { return _sessionId; }
	void		 SetSessionId(SessionID sessionId) { _sessionId = sessionId; }

private:
	SessionID _sessionId;

	bool onRecv(const PacketHeader& header, CHAR* dataStartPtr);
	void recv(DWORD recvBytes);

	void attachNetwork(shared_ptr<TcpNetwork> network);
	void detachNetwork();
};