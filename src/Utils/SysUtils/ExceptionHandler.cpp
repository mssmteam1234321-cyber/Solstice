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

    std::string excPath = FileUtils::getSolsticeDir() + "crash.log";
    // create if not exists
    if (!FileUtils::fileExists(excPath))
    {
        std::ofstream excFile(excPath);
        excFile.close();
    }

    std::ofstream excFile(excPath, std::ios::app);
    if (excFile.is_open())
    {
        // Prepend the date and time to the crash log
        auto now = std::chrono::system_clock::now();
        auto nowTime = std::chrono::system_clock::to_time_t(now);
        excFile << "----------------- Crash at " << std::put_time(std::localtime(&nowTime), "%Y-%m-%d %X") << "\n";
        excFile << text << "\n\n";
        auto modules = gFeatureManager->mModuleManager->getModules();
        for (const auto& module : modules)
        {
            // Append modules and the mEnabled state
            excFile << module->mName << " - " << (module->mEnabled ? "Enabled" : "Disabled") << "\n";
        }

        // Append the exception code then divider
        excFile << "Exception Code: 0x" << fmt::format("{:X}", exceptionCode) << "\n";
        excFile << "----------------------------------------\n";
        excFile.close();
    }

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
