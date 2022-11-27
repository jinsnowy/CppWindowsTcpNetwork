#pragma once

#include "Singleton.h"
#include "DBConnection.h"

class DBConnectionPool : public ISingleton<DBConnectionPool>
{
public:
	DBConnectionPool();
	~DBConnectionPool();

	bool Connect(int32 connectionCount, LPCWSTR connectionString);
	void Clear();

	DBConnection* Pop();
	void Push(DBConnection* connection);

private:
	StdMutex _mtx;
	SQLHENV  _environment = SQL_NULL_HANDLE;
	vector<DBConnection*> _connections;
};

