#pragma once

namespace Engine
{
    class MiniDump
    {
    public:
        static LONG WINAPI UnHandledExceptionFilter(struct _EXCEPTION_POINTERS* exceptionInfo);
        static BOOL BeginDump();
    };
}