#pragma once

namespace Engine
{
    class MiniDump
    {
    public:
        static LONG WINAPI unHandledExceptionFilter(struct _EXCEPTION_POINTERS* exceptionInfo);
        static BOOL beginDump();
    };
}