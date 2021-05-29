
#include <SDKDDKVer.h>

#include "Editor/Editor.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "resource.h"

#define MAX_LOADSTRING 100

HWND hwnd;

const char* OpenFileDialog(const char* extName, const char* ext, bool open)
{
    char curDir[512];
    GetCurrentDirectoryA(512, curDir);

    char curDirDialog[512];
    Oak::StringUtils::Copy(curDirDialog, 512, curDir);

    OPENFILENAMEA ofn;

    static char fileName[512];

    char filter[512];
    Oak::StringUtils::Copy(filter, 512, extName);

    int index = (int)strlen(filter);

    filter[index + 1] = '*';
    filter[index + 2] = '.';

    if (ext)
    {
        Oak::StringUtils::Copy(&filter[index + 3], 5, ext);
    }
    else
    {
        Oak::StringUtils::Copy(&filter[index + 3], 5, "*");
    }

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = fileName;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 512;
    ofn.lpstrFilter = filter;
    ofn.lpstrDefExt = ext;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = curDirDialog;
    ofn.Flags = OFN_PATHMUSTEXIST;

    if (open)
    {
        ofn.Flags |= OFN_FILEMUSTEXIST;
    }

    bool res = false;

    if (open)
    {
        res = GetOpenFileNameA(&ofn) ? true : false;
    }
    else
    {
        res = GetSaveFileNameA(&ofn) ? true : false;
    }

    SetCurrentDirectoryA(curDir);

    if (res)
    {
        return fileName;
    }

    return nullptr;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (Oak::editor.ProcHandler(hWnd, msg, wParam, lParam))
    {
        return true;
    }

    switch (msg)
    {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            Oak::editor.OnResize((int)LOWORD(lParam), (int)HIWORD(lParam));
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

    hwnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);

    if (!hwnd)
    {
        return 1;
    }

    if (!Oak::editor.Init(hwnd))
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

        if (!Oak::editor.Update())
        {
            break;
        }
    }

    Oak::editor.Release();

    DestroyWindow(hwnd);
    UnregisterClass(wcex.lpszClassName, wcex.hInstance);

    return 0;
}