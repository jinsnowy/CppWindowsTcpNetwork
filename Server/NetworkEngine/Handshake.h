#pragma once

class TcpNetwork;
class Handshake
{
protected:
	weak_ptr<TcpNetwork>		_network;

public:
	Handshake(shared_ptr<TcpNetwork> network);

	void Process();

protected:
	virtual void onProcess(shared_ptr<TcpNetwork> network) abstract;
};

class ClientHandshake : public Handshake
{
public:
	ClientHandshake(shared_ptr<TcpNetwork> network);

protected:
	virtual void onProcess(shared_ptr<TcpNetwork> network) override;
};

class ServerHandshake : public Handshake
{
public:
	ServerHandshake(shared_ptr<TcpNetwork> network);

protected:
	virtual void onProcess(shared_ptr<TcpNetwork> network) override;
};