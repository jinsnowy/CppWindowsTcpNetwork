#pragma once
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

class DateTime : protected tm
{
public:
	static DateTime now();
	static DateTime utcNow();

	int getYear() const { return tm_year + 1900; }
	int getDays() const { return tm_mday; }
	int getMonth() const { return tm_mon + 1; }

	std::string toString(const char* format = "%Y-%m-%d %H:%M:%S") const;

	std::string toSqlString() const;
};