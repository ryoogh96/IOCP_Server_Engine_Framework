#pragma once
#include "DBConnection.hpp"

namespace Engine
{
	class DBConnectionPool
	{
	public:
		DBConnectionPool();
		~DBConnectionPool();

		bool					Connect(int32 connectionCount, const WCHAR* connectionString);
		void					Clear();

		DBConnection* Pop();
		void					Push(DBConnection* connection);

	private:
		USE_LOCK;
		SQLHENV					m_Environment = SQL_NULL_HANDLE;
		Vector<DBConnection*>	m_Connections;
	};
}