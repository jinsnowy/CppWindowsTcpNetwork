#pragma once
#include "Session.h"

class ClientSession : public Session
{
public:
	ClientSession();

	void SetConnector(shared_ptr<TcpNetwork> connector);

	bool Connect(const char* address, uint16 port);

	bool Connect(const EndPoint& endPoint);

	bool Reconnect(const EndPoint& endPoint);

	void ConnectAsync(const EndPoint& endPoint);

	void ReconnectAsync(const EndPoint& endPoint);

	bool IsLogin() { return _logined; }
protected:
	virtual void onConnected() override;

	virtual void onDisconnected() override;

	virtual void onAuthorized() override;

private:
	shared_ptr<TcpNetwork> _connector;
	
	atomic<bool> _logined;
};