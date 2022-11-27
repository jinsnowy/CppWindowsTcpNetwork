#include "pch.h"

DateTime DateTime::Now()
{
	auto t = std::time(NULL);
	auto date = DateTime();
	localtime_s((tm*)&date, &t);

	return date;
}

DateTime DateTime::UtcNow()
{
	auto t = std::time(NULL);
	auto date = DateTime();
	gmtime_s((tm*)&date, &t);

	return date;
}

std::string DateTime::toString(const char* format) const
{
	std::ostringstream oss;
	oss << std::put_time((tm*)this, format);
	return oss.str();
}

std::string DateTime::toSqlString() const
{
	return toString("%Y-%m-%d %H:%M:%S");
}