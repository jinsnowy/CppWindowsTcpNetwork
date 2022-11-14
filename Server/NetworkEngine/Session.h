#pragma once
#include "TcpSocket.h"
#include "RecvBuffer.h"

class Session : public std::enable_shared_from_this<Session>
{
	friend class Listener;
	friend struct on_accept_t;
	friend struct on_connect_t;
	friend struct on_disconnect_t;
	friend struct on_recv_t;
	friend struct on_send_t;
public:
	Session(IoService& ioService);
	virtual ~Session();

public:
	void sendAsync(const BufferSegment& segment);

	template<typename T>
	shared_ptr<T> getShared() { return static_pointer_cast<T>(shared_from_this()); }

	SOCKET getSocket() { return _socket.getSocket(); };

	RecvBuffer& getRecvBuffer() { return _recvBuffer; }

	bool isConnected() { return _connected; }

	void disconnect();

	bool connect(const char* address, uint16 port);

	bool connect(const EndPoint& endPoint);

	bool reconnect(const EndPoint& endPoint);

	bool connectAsync(const EndPoint& endPoint);

	bool reconnectAsync(const EndPoint& endPoint);

protected:
	TcpActiveSocket _socket;

	virtual void onConnected();
	
	virtual void onDisconnected();

	virtual void onRecv(DWORD bytes);

public:
	EndPoint getEndPoint() { return _endPoint; }
	string   getEndPointDesc() { return _endPoint.toString(); }

private:
	atomic<bool> _connected;
	atomic<bool> _sendIsPending;
	EndPoint	 _endPoint;

	StdLock				    _sync;
	vector<BufferSegment>	_pendingSegment;
	RecvBuffer				_recvBuffer;
private:
	void flush();
	bool flushInternal();
	void registerRecv();
	void disconnectOnError(const char* reason = "");

	void setEndPoint(const EndPoint& endPoint) { _endPoint = endPoint; }
	void setDisconnected();
	void setConnected();
	void handleError(int32 errorCode);

	void recv(DWORD recvBytes);
};
