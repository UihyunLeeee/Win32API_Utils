#pragma once
#include <windows.h>
#include <string> // Used for std::wstring in the PrintErrorMsg implementation

class ConsoleDebugger
{
public:
    enum class Status
    {
        Success = 0,
        Error_FailedToGetHandle = -1,
        Error_FailedToAllocConsole = -2
    };

private:
    // Your member variables
    HANDLE m_hConsole;
    Status m_status;

public:
    // Method Declarations
    ConsoleDebugger();
    ~ConsoleDebugger();

    Status GetStatus() const;
    bool IsValid() const;
    HANDLE GetHandle() const;
    void PrintErrorMsg(const wchar_t* message) const;
};