
#include <SDKDDKVer.h>

#include "Platform/Common/IRunner.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "../Projects/Windows/Resources/resource.h"

#define MAX_LOADSTRING 100

HWND hwnd;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            Orin::GetRunner()->OnResize((int)LOWORD(lParam), (int)HIWORD(lParam));
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
    WCHAR szTitle[MAX_LOADSTRING];
    WCHAR szWindowClass[MAX_LOADSTRING];

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OAK, szWindowClass, MAX_LOADSTRING);

    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OAK));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    wcex.lpszMenuName = nullptr;

    RegisterClassExW(&wcex);

    RECT desktopRect;
    HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktopRect);

    hwnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, desktopRect.right, desktopRect.bottom, nullptr, nullptr, hInstance, nullptr);

    if (!hwnd)
    {
        return 1;
    }

    SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
    SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, desktopRect.right, desktopRect.bottom, SWP_SHOWWINDOW);

    if (!Orin::GetRunner()->Init(hwnd))
    {
        UnregisterClass(wcex.lpszClassName, wcex.hInstance);
        return 1;
    }

    ShowWindow(hwnd, SW_NORMAL);
    UpdateWindow(hwnd);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }

        Orin::GetRunner()->Update();

        if (Orin::GetRunner()->IsQuitRequested())
        {
            break;
        }
    }

    Orin::GetRunner()->Release();

    DestroyWindow(hwnd);
    UnregisterClass(wcex.lpszClassName, wcex.hInstance);

    return 0;
}