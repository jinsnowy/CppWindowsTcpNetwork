#pragma once
#include <exception>
#include <string>

class PacketException : public std::exception
{
private:
	std::string _message;

public:
	PacketException(const std::string& message) : _message(message) {}

	virtual const char* what() const override { return _message.c_str(); }
};

class HandshakeFailExcpetion : public PacketException
{
public:
	HandshakeFailExcpetion() : PacketException("handshake failed") {}
};