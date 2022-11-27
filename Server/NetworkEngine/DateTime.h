#pragma once
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

class DateTime : protected tm
{
public:
	static DateTime Now();
	static DateTime UtcNow();

	int Year() const { return tm_year + 1900; }
	int Day() const { return tm_mday; }
	int Month() const { return tm_mon + 1; }

	std::string toString(const char* format = "%Y-%m-%d %H:%M:%S") const;

	std::string toSqlString() const;
};