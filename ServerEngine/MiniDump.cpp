#include "pch.hpp"
#include "ServerEnginePCH.hpp"

namespace Engine
{
    void printLastError(DWORD dwError = GetLastError())
    {
        TCHAR* lpOSMsg;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpOSMsg, 0, nullptr);

        _tprintf(_T("[ERROR] [%d] %s\n"), dwError, lpOSMsg);
        LocalFree(lpOSMsg);
    }

    typedef BOOL(WINAPI* MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType, const PMINIDUMP_EXCEPTION_INFORMATION ExpcetionParam, const PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, const PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

    LONG WINAPI MiniDump::unHandledExceptionFilter(struct _EXCEPTION_POINTERS* exceptionInfo)
    {
        HMODULE DLLHandle = LoadLibrary(_T("DBGHELP.DLL"));
        if (nullptr == DLLHandle) {
            printLastError();
            return EXCEPTION_CONTINUE_SEARCH;
        }

        MINIDUMPWRITEDUMP dump = (MINIDUMPWRITEDUMP)GetProcAddress(DLLHandle, "MiniDumpWriteDump");
        if (nullptr == dump) {
            printLastError();
            return EXCEPTION_CONTINUE_SEARCH;
        }

        SYSTEMTIME systemTime;
        GetLocalTime(&systemTime);

        TCHAR dumpPath[MAX_PATH] = { 0, };

        _sntprintf_s(dumpPath, MAX_PATH, _TRUNCATE, _T("%d-%d-%d %d_%d_%d.dmp"), systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond);

        HANDLE fileHandle = CreateFile(dumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

        if (INVALID_HANDLE_VALUE == fileHandle)
        {
            printLastError();
            return EXCEPTION_CONTINUE_SEARCH;
        }

        _MINIDUMP_EXCEPTION_INFORMATION miniDumpExceptionInfo;
        miniDumpExceptionInfo.ThreadId = GetCurrentThreadId();
        miniDumpExceptionInfo.ExceptionPointers = (PEXCEPTION_POINTERS)exceptionInfo;
        miniDumpExceptionInfo.ClientPointers = NULL;

        BOOL bSuccess = dump(GetCurrentProcess(), GetCurrentProcessId(), fileHandle, MiniDumpNormal, &miniDumpExceptionInfo, nullptr, nullptr);

        CloseHandle(fileHandle);

        return (TRUE == bSuccess) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;
    }

    BOOL MiniDump::beginDump()
    {
        SetErrorMode(SEM_FAILCRITICALERRORS);

        LPTOP_LEVEL_EXCEPTION_FILTER previousExceptionFilter = nullptr;
        previousExceptionFilter = SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)unHandledExceptionFilter);

        return TRUE;
    }
}