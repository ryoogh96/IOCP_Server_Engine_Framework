#pragma once
#include "DBConnection.hpp"

namespace Engine
{
	template<int32 C>
	struct FullBits { enum { value = (1 << (C - 1)) | FullBits<C - 1>::value }; };

	template<>
	struct FullBits<1> { enum { value = 1 }; };

	template<>
	struct FullBits<0> { enum { value = 0 }; };

	template<int32 ParamCount, int32 ColumnCount>
	class DBBind
	{
	public:
		DBBind(DBConnection& dbConnection, const WCHAR* query)
			: m_DBConnection(dbConnection), _query(query)
		{
			::memset(m_ParamIndex, 0, sizeof(m_ParamIndex));
			::memset(m_ColumnIndex, 0, sizeof(m_ColumnIndex));
			m_ParamFlag = 0;
			m_ColumnFlag = 0;
			dbConnection.Unbind();
		}

		bool Validate()
		{
			return m_ParamFlag == FullBits<ParamCount>::value && m_ColumnFlag == FullBits<ColumnCount>::value;
		}

		bool Execute()
		{
			ASSERT_CRASH(Validate());
			return m_DBConnection.Execute(_query);
		}

		bool Fetch()
		{
			return m_DBConnection.Fetch();
		}

	public:
		template<typename T>
		void BindParam(int32 idx, T& value)
		{
			m_DBConnection.BindParam(idx + 1, &value, &m_ParamIndex[idx]);
			m_ParamFlag |= (1LL << idx);
		}

		void BindParam(int32 idx, const WCHAR* value)
		{
			m_DBConnection.BindParam(idx + 1, value, &m_ParamIndex[idx]);
			m_ParamFlag |= (1LL << idx);
		}

		template<typename T, int32 N>
		void BindParam(int32 idx, T(&value)[N])
		{
			m_DBConnection.BindParam(idx + 1, (const BYTE*)value, size32(T) * N, &m_ParamIndex[idx]);
			m_ParamFlag |= (1LL << idx);
		}

		template<typename T>
		void BindParam(int32 idx, T* value, int32 N)
		{
			m_DBConnection.BindParam(idx + 1, (const BYTE*)value, size32(T) * N, &m_ParamIndex[idx]);
			m_ParamFlag |= (1LL << idx);
		}

		template<typename T>
		void BindCol(int32 idx, T& value)
		{
			m_DBConnection.BindCol(idx + 1, &value, &m_ColumnIndex[idx]);
			m_ColumnFlag |= (1LL << idx);
		}

		template<int32 N>
		void BindCol(int32 idx, WCHAR(&value)[N])
		{
			m_DBConnection.BindCol(idx + 1, value, N - 1, &m_ColumnIndex[idx]);
			m_ColumnFlag |= (1LL << idx);
		}

		void BindCol(int32 idx, WCHAR* value, int32 len)
		{
			m_DBConnection.BindCol(idx + 1, value, len - 1, &m_ColumnIndex[idx]);
			m_ColumnFlag |= (1LL << idx);
		}

		template<typename T, int32 N>
		void BindCol(int32 idx, T(&value)[N])
		{
			m_DBConnection.BindCol(idx + 1, value, size32(T) * N, &m_ColumnIndex[idx]);
			m_ColumnFlag |= (1LL << idx);
		}

	protected:
		DBConnection& m_DBConnection;
		const WCHAR* _query;
		SQLLEN			m_ParamIndex[ParamCount > 0 ? ParamCount : 1];
		SQLLEN			m_ColumnIndex[ColumnCount > 0 ? ColumnCount : 1];
		uint64			m_ParamFlag;
		uint64			m_ColumnFlag;
	};
}