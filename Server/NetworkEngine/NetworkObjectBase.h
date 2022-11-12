#pragma once
class Logger;
class NetworkObjectBase
{
private:
	unsigned long long mId;

protected:
	NetworkObjectBase();
	virtual ~NetworkObjectBase() = default;

public:
	virtual std::string toString() const { return ""; };
};