#include "pch.h"
#include "DBConnectionPool.h"

DBConnectionPool::DBConnectionPool()
{
}

DBConnectionPool::~DBConnectionPool()
{
	Clear();
}

bool DBConnectionPool::Connect(int32 connectionCount, LPCWSTR connectionString)
{
	StdWriteLock lk(_mtx);

	if (::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_environment) != SQL_SUCCESS)
		return false;

	if (::SQLSetEnvAttr(_environment, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 0) != SQL_SUCCESS)
		return false;

	for (int32 i = 0; i < connectionCount; ++i)
	{
		DBConnection* conn = new DBConnection();
		if (conn->Connect(_environment, connectionString) == false)
			return false;
		
		_connections.push_back(conn);
	}

	return true;
}

void DBConnectionPool::Clear()
{
	StdWriteLock lk(_mtx);

	if (_environment != SQL_NULL_HANDLE)
	{
		::SQLFreeHandle(SQL_HANDLE_ENV, _environment);
		_environment = SQL_NULL_HANDLE;
	}

	int connectionCount = (int)_connections.size();
	for (int i = 0; i < connectionCount; ++i)
	{
		delete _connections[i];
	}

	_connections.clear();
}

DBConnection* DBConnectionPool::Pop()
{
	StdWriteLock lk(_mtx);

	if (_connections.empty())
		return nullptr;

	auto conn = _connections.back();
	_connections.pop_back();

	return conn;
}

void DBConnectionPool::Push(DBConnection* connection)
{
	StdWriteLock lk(_mtx);
	_connections.push_back(connection);
}
