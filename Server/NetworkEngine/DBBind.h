#pragma once
#include "DBConnection.h"

template<int32 ParamCount, int32 ColumnCount>
class DBBind
{
public:
	DBBind(DBConnection& dbConnection, LPCWSTR query)
		:
		_dbConnection(dbConnection),
		_query(query)
	{
		ZeroMemory(_paramIndex, sizeof(_paramIndex));
		ZeroMemory(_columnIndex, sizeof(_columnIndex));
		_paramFlag.reset();
		_columnFlag.reset();
		dbConnection.Unbind();
	}

	bool Validate()
	{
		return _paramFlag.all() && _columnFlag.all();
	}

	bool Execute()
	{
		ASSERT_CRASH(Validate());
		return _dbConnection.Execute(_query);
	}

	bool Fetch()
	{
		return _dbConnection.Fetch();
	}

public:
	template<typename T>
	void BindParam(int32 idx, T& value)
	{
		_dbConnection.BindParam(idx + 1, &value, &_paramIndex[idx]);
		_paramFlag.set(idx);
	}

	void BindParam(int32 idx, LPCWSTR str)
	{
		_dbConnection.BindParam(idx + 1, str, & _paramIndex[idx]);
		_paramFlag.set(idx);
	}

	template<typename T, int32 NArr>
	void BindParam(int32 idx, T(&value)[NArr])
	{
		_dbConnection.BindParam(idx + 1, (BYTE*)value, size32(T) * NArr, &_paramIndex[idx]);
		_paramFlag.set(idx);
	}

	template<typename T>
	void BindCol(int32 idx, T& value)
	{
		_dbConnection.BindCol(idx + 1, &value, &_columnIndex[idx]);
		_columnFlag.set(idx);
	}

	void BindCol(int32 idx, LPCWSTR str)
	{
		_dbConnection.BindCol(idx + 1, &str, &_columnIndex[idx]);
		_columnFlag.set(idx);
	}

	template<typename T, int32 NArr>
	void BindCol(int32 idx, T(&value)[NArr])
	{
		_dbConnection.BindCol(idx + 1, (BYTE*)value, size32(T) * NArr, &_columnIndex[idx]);
		_columnFlag.set(idx);
	}

protected:
	DBConnection& _dbConnection;
	LPCWSTR		  _query;
	SQLLEN		  _paramIndex[ParamCount > 0 ? ParamCount : 1];
	SQLLEN		  _columnIndex[ColumnCount > 0 ? ColumnCount : 1];
	bitset<ParamCount>  _paramFlag;
	bitset<ColumnCount> _columnFlag;
};

