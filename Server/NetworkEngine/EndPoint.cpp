#include "pch.h"
#include "EndPoint.h"

std::string EndPoint::ToString() const
{
	return Format::format("%s:%s", GetIpAddress().c_str(), GetPort().c_str());
}