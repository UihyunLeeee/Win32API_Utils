#include "Util.h"    // Include the class declaration
#include <stdio.h>   // For freopen_s
#include <locale.h>  // For _wsetlocale
#include <chrono>    // For getting the current time
#include <ctime>     // For formatting the time into a string

// Constructor implementation
ConsoleDebugger::ConsoleDebugger() : m_hConsole(NULL), m_status(Status::Success)
{
    if (AllocConsole())
    {
        m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        if (m_hConsole == INVALID_HANDLE_VALUE)
        {
            m_status = Status::Error_FailedToGetHandle;
            return;
        }

        FILE* pDummy;
        if (freopen_s(&pDummy, "CONOUT$", "w", stdout) != 0 ||
            freopen_s(&pDummy, "CONIN$", "r", stdin) != 0)
        {
            m_status = Status::Error_FailedToGetHandle;
            return;
        }

        _wsetlocale(LC_ALL, L"");
    }
    else
    {
        m_status = Status::Error_FailedToAllocConsole;
    }
}

// Destructor implementation
ConsoleDebugger::~ConsoleDebugger()
{
    if (m_status == Status::Success)
    {
        FreeConsole();
    }
}

// Getter implementations
ConsoleDebugger::Status ConsoleDebugger::GetStatus() const
{
    return m_status;
}

bool ConsoleDebugger::IsValid() const
{
    return m_status == Status::Success;
}

HANDLE ConsoleDebugger::GetHandle() const
{
    return m_hConsole;
}

// PrintErrorMsg implementation with timestamp
void ConsoleDebugger::PrintErrorMsg(const wchar_t* message) const
{
    if (!this->IsValid()) {
        return;
    }

    // 1. Get current time
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

    // 2. Convert to calendar time structure
    tm local_tm;
    localtime_s(&local_tm, &now_time_t);

    // 3. Format the time into a wide string
    wchar_t time_buffer[100];
    wcsftime(time_buffer, 100, L"[%Y-%m-%d %H:%M:%S] ", &local_tm);

    // 4. Combine timestamp and message using std::wstring
    std::wstring full_message = time_buffer;
    full_message += message;
    full_message += L"\n";

    // 5. Write the final string to the console
    DWORD chars_written;
    WriteConsoleW(
        m_hConsole,
        full_message.c_str(),
        (DWORD)full_message.length(),
        &chars_written,
        NULL
    );
}