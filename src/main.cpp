#include "config.h"
#include "Util.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

wchar_t *buf;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PWSTR pCmdLine, int nShowCmd)
{
#ifdef MY_APP_DEBUG_MODE
    ConsoleDebugger debugger;
    if (!debugger.IsValid())
    {
        debugger.PrintErrorMsg(L"Failed to initialize console debugger.");
        return -1;
    }
    debugger.PrintErrorMsg(L"Console debugger initialized successfully.");
#endif

    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASSW wc = { };
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassW(&wc))
    {
        MessageBoxW(NULL, L"Window Registration Failed!", L"Error", MB_ICONERROR | MB_OK);
        return 0;
    }

    HWND hWnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"My Simple Text Editor",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL
    );

    if (hWnd == NULL)
    {
        return 0;
    }

    ShowWindow(hWnd, nShowCmd);
    UpdateWindow(hWnd);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    int len;

    switch (uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}