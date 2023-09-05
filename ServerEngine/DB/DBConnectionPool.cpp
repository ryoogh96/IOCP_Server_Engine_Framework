#include "pch.hpp"
#include "DBConnectionPool.hpp"

namespace Engine
{
	DBConnectionPool::DBConnectionPool()
	{

	}

	DBConnectionPool::~DBConnectionPool()
	{
		Clear();
	}

	bool DBConnectionPool::Connect(int32 connectionCount, const WCHAR* connectionString)
	{
		WRITE_LOCK;

		if (::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_Environment) != SQL_SUCCESS)
			return false;

		if (::SQLSetEnvAttr(m_Environment, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 0) != SQL_SUCCESS)
			return false;

		for (int32 i = 0; i < connectionCount; i++)
		{
			DBConnection* connection = xnew<DBConnection>();
			if (connection->Connect(m_Environment, connectionString) == false)
				return false;

			m_Connections.push_back(connection);
		}

		return true;
	}

	void DBConnectionPool::Clear()
	{
		WRITE_LOCK;

		if (m_Environment != SQL_NULL_HANDLE)
		{
			::SQLFreeHandle(SQL_HANDLE_ENV, m_Environment);
			m_Environment = SQL_NULL_HANDLE;
		}

		for (DBConnection* connection : m_Connections)
			xdelete(connection);

		m_Connections.clear();
	}

	DBConnection* DBConnectionPool::Pop()
	{
		WRITE_LOCK;

		if (m_Connections.empty())
			return nullptr;

		DBConnection* connection = m_Connections.back();
		m_Connections.pop_back();
		return connection;
	}

	void DBConnectionPool::Push(DBConnection* connection)
	{
		WRITE_LOCK;
		m_Connections.push_back(connection);
	}
}