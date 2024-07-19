//
// Created by vastrakai on 7/7/2024.
//

#include "ExceptionHandler.hpp"
#include <Windows.h>
#include <string>
#include <spdlog/spdlog.h>

#include "StackWalker.hpp"

LONG WINAPI TopLevelExceptionHandler(const PEXCEPTION_POINTERS pExceptionInfo)
{
    // Get the exception code
    auto exceptionCode = pExceptionInfo->ExceptionRecord->ExceptionCode;

    std::string text = "An unhandled exception occurred. (0x" + fmt::format("{:X}", exceptionCode) + ")\nStack Trace:\n";

    // Get the stack trace using stackwalker
    StackWalker sw;
    std::vector<std::string> stackTrace = sw.ShowCallstack(GetCurrentThread(), pExceptionInfo->ContextRecord);
    for (const auto& line : stackTrace)
    {
        text += line + "\n";
    }
    spdlog::error(text);

    auto result = MessageBoxA(nullptr, LPCSTR(text.c_str()), "Unhandled Exception", MB_ABORTRETRYIGNORE | MB_ICONERROR);

    // deconstruct stack trace
    stackTrace.clear();
    sw.~StackWalker();


    switch (result)
    {
    case IDABORT:   // Terminates the process
        exit(1);
    case IDRETRY:
        return EXCEPTION_CONTINUE_EXECUTION;
    case IDIGNORE:
        return EXCEPTION_CONTINUE_SEARCH;
    default:
        return EXCEPTION_EXECUTE_HANDLER;
    }
    return EXCEPTION_EXECUTE_HANDLER;
}

void ExceptionHandler::init()
{
    SetUnhandledExceptionFilter(TopLevelExceptionHandler);
}
