#include "pch.hpp"
#include "DBConnection.hpp"

namespace Engine
{
	bool DBConnection::Connect(SQLHENV henv, const WCHAR* connectionString)
	{
		if (::SQLAllocHandle(SQL_HANDLE_DBC, henv, &m_Connection) != SQL_SUCCESS)
			return false;

		WCHAR stringBuffer[MAX_PATH] = { 0 };
		::wcscpy_s(stringBuffer, connectionString);

		WCHAR resultString[MAX_PATH] = { 0 };
		SQLSMALLINT resultStringLen = 0;

		SQLRETURN ret = ::SQLDriverConnectW(
			m_Connection,
			NULL,
			reinterpret_cast<SQLWCHAR*>(stringBuffer),
			_countof(stringBuffer),
			OUT reinterpret_cast<SQLWCHAR*>(resultString),
			_countof(resultString),
			OUT & resultStringLen,
			SQL_DRIVER_NOPROMPT
		);

		if (::SQLAllocHandle(SQL_HANDLE_STMT, m_Connection, &m_Statement) != SQL_SUCCESS)
			return false;

		return (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
	}

	void DBConnection::Clear()
	{
		if (m_Connection != SQL_NULL_HANDLE)
		{
			::SQLFreeHandle(SQL_HANDLE_DBC, m_Connection);
			m_Connection = SQL_NULL_HANDLE;
		}

		if (m_Statement != SQL_NULL_HANDLE)
		{
			::SQLFreeHandle(SQL_HANDLE_STMT, m_Statement);
			m_Statement = SQL_NULL_HANDLE;
		}
	}

	bool DBConnection::Execute(const WCHAR* query)
	{
		SQLRETURN ret = ::SQLExecDirectW(m_Statement, (SQLWCHAR*)query, SQL_NTSL);
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
			return true;

		HandleError(ret);
		return false;
	}

	bool DBConnection::Fetch()
	{
		SQLRETURN ret = ::SQLFetch(m_Statement);

		switch (ret)
		{
		case SQL_SUCCESS:
		case SQL_SUCCESS_WITH_INFO:
			return true;
		case SQL_NO_DATA:
			return false;
		case SQL_ERROR:
			HandleError(ret);
			return false;
		default:
			return true;
		}
	}

	int32 DBConnection::GetRowCount()
	{
		SQLLEN count = 0;
		SQLRETURN ret = ::SQLRowCount(m_Statement, OUT & count);

		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
			return static_cast<int32>(count);

		return -1;
	}

	void DBConnection::Unbind()
	{
		::SQLFreeStmt(m_Statement, SQL_UNBIND);
		::SQLFreeStmt(m_Statement, SQL_RESET_PARAMS);
		::SQLFreeStmt(m_Statement, SQL_CLOSE);
	}

	bool DBConnection::BindParam(SQLUSMALLINT paramIndex, SQLSMALLINT cType, SQLSMALLINT sqlType, SQLULEN len, SQLPOINTER ptr, SQLLEN* index)
	{
		SQLRETURN ret = ::SQLBindParameter(m_Statement, paramIndex, SQL_PARAM_INPUT, cType, sqlType, len, 0, ptr, 0, index);
		if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
		{
			HandleError(ret);
			return false;
		}

		return true;
	}

	bool DBConnection::BindCol(SQLUSMALLINT columnIndex, SQLSMALLINT cType, SQLULEN len, SQLPOINTER value, SQLLEN* index)
	{
		SQLRETURN ret = ::SQLBindCol(m_Statement, columnIndex, cType, value, len, index);
		if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
		{
			HandleError(ret);
			return false;
		}

		return true;
	}

	void DBConnection::HandleError(SQLRETURN ret)
	{
		if (ret == SQL_SUCCESS)
			return;

		SQLSMALLINT index = 1;
		SQLWCHAR sqlState[MAX_PATH] = { 0 };
		SQLINTEGER nativeErr = 0;
		SQLWCHAR errMsg[MAX_PATH] = { 0 };
		SQLSMALLINT msgLen = 0;
		SQLRETURN errorRet = 0;

		while (true)
		{
			errorRet = ::SQLGetDiagRecW(
				SQL_HANDLE_STMT,
				m_Statement,
				index,
				sqlState,
				OUT & nativeErr,
				errMsg,
				_countof(errMsg),
				OUT & msgLen
			);

			if (errorRet == SQL_NO_DATA)
				break;

			if (errorRet != SQL_SUCCESS && errorRet != SQL_SUCCESS_WITH_INFO)
				break;

			std::wcout.imbue(std::locale("kor"));
			std::wcout << errMsg << std::endl;

			index++;
		}
	}
}