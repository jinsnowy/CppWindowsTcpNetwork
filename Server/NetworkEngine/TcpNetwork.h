#include "TcpSocket.h"
#include "RecvBuffer.h"

class Handshake;
class TcpNetwork : public std::enable_shared_from_this<TcpNetwork>
{
	friend class Listener;
	friend struct on_accept_t;
	friend struct on_connect_t;
	friend struct on_disconnect_t;
	friend struct on_recv_t;
	friend struct on_send_t;
private:
	TcpActiveSocket _socket;
	SessionWeakPtr  _session;
	EndPoint		_endPoint;
	unique_ptr<Handshake>  _handshake;

public:
	TcpNetwork(ServiceBase& ServiceBase);

	~TcpNetwork();

	void AttachSession(SessionPtr session);

	void ProcessHandshake();

	void RequireHandshake(Handshake* handshake);

	void SendAsync(const BufferSegment& segment);

	void DisconnectAsync();
	
	void ConnectAsync(const EndPoint& endPoint);

	void Start() { registerRecv(); }
public:
	SessionPtr GetSession() { return _session.lock(); }

	SOCKET GetSocket() { return _socket.GetSocket(); };

	RecvBuffer& GetRecvBuffer() { return _recvBuffer; }

	EndPoint GetEndPoint() { return _endPoint; }

	bool IsConnected() { return _connected; }

private:
	void setDisconnected();

	void setConnected(EndPoint endPoint);

	void recv(DWORD bytes);

	void flush();
	
	bool flushInternal();
	
	void registerRecv(bool init = false);

	void disconnectOnError(const char* reason = "");

	void handleError(int32 errorCode);
private:
	atomic<bool>			_connected;
	atomic<bool>			_pending;
	StdMutex				_sync;
	vector<BufferSegment>	_pendingSegment;
	RecvBuffer				_recvBuffer;

private:
	StdMutex				    _handlerSync;
	vector<PacketHandler*>  _packetHandlers;

	bool resolvePacketHandler(int32 protocol, PacketHandler** handler);

public:
	template<typename T>
	void InstallPacketHandler()
	{
		StdWriteLock lk(_handlerSync);

		for (auto& packetHandler : _packetHandlers)
		{
			if (dynamic_cast<T*>(packetHandler) != nullptr)
				return;
		}

		_packetHandlers.push_back(PacketHandler::GetHandler<T>());
	}

	template<typename T>
	void UninstallPacketHandler()
	{
		StdWriteLock lk(_handlerSync);

		auto iter = std::find_if(_packetHandlers.begin(), _packetHandlers.end(), [](PacketHandler* _packetHandler)
		{
			return dynamic_cast<T*>(_packetHandler) != nullptr;
		});

		if (iter != _packetHandlers.end())
		{
			_packetHandlers.erase(iter);
		}
	}
};