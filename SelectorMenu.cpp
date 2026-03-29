#include <windows.h>
#include <dwmapi.h>
#include <vector>
#include <string>
#include <gdiplus.h>
#include <algorithm>
#include <tlhelp32.h>

// --- LINKER BEÁLLÍTÁSOK ---
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "dwmapi.lib")
// #pragma comment(lib, "CheatSelector.lib") // Ha külön fájlban van a logikád

// --- STRUKTÚRÁK ÉS ADATOK ---
struct GameEntry {
    int id;
    std::wstring name;
    std::wstring processName;
    std::string dllPath; // A játékhoz tartozó egyedi DLL
};

struct HaxEntry {
    int gameId;
    std::wstring name;
};

// Globális listák (Ezt később kihelyezheted Games.h-ba)
std::vector<GameEntry> GamesList = {
    { 1, L"Minecraft", L"javaw.exe", "C:\\GG\\mc_hax.dll" },
    { 2, L"Fortnite", L"FortniteClient-Win64-Shipping.exe", "C:\\GG\\fn_hax.dll" },
    { 3, L"Notepad Teszt", L"notepad.exe", "C:\\GG\\test_hax.dll" }
};

std::vector<HaxEntry> HaxList = {
    { 1, L"Killaura & Fly" },
    { 1, L"X-Ray Vision" },
    { 2, L"Aimbot (Safe)" },
    { 3, L"Test Injection" }
};

// Állapotjelzők
namespace GlobalState {
    int selectedGameId = -1;
    bool IsGameOpened = false;
    std::string License = "Premium"; // Példa teszteléshez
}

// --- INJECTOR LOGIKA ---
namespace Injector {
    DWORD GetPidByName(const wchar_t* processName) {
        DWORD pid = 0;
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32W entry;
            entry.dwSize = sizeof(entry);
            if (Process32FirstW(snapshot, &entry)) {
                do {
                    if (_wcsicmp(entry.szExeFile, processName) == 0) {
                        pid = entry.th32ProcessID;
                        break;
                    }
                } while (Process32NextW(snapshot, &entry));
            }
            CloseHandle(snapshot);
        }
        return pid;
    }

    bool RunInjection(DWORD pid, const char* dllPath) {
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        if (!hProcess) return false;

        LPVOID loadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
        size_t pathLen = strlen(dllPath) + 1;

        LPVOID remoteMem = VirtualAllocEx(hProcess, NULL, pathLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!remoteMem) { CloseHandle(hProcess); return false; }

        WriteProcessMemory(hProcess, remoteMem, dllPath, pathLen, NULL);
        HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibAddr, remoteMem, 0, NULL);

        if (hThread) {
            WaitForSingleObject(hThread, INFINITE);
            CloseHandle(hThread);
            VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            return true;
        }
        CloseHandle(hProcess);
        return false;
    }
}

// --- UI GRAFIKA ---
namespace SelectorMenu {
    struct ClickableButton {
        Gdiplus::Rect rect;
        int gameId;
        bool isHax;
    };

    std::vector<ClickableButton> ActiveButtons;

    // Színpaletta
    Gdiplus::Color COL_BG(255, 15, 15, 20);
    Gdiplus::Color COL_SIDEBAR(255, 25, 25, 30);
    Gdiplus::Color COL_ACCENT(255, 99, 102, 241);
    Gdiplus::Color COL_BTN(255, 45, 45, 55);
    Gdiplus::Color COL_ONLINE(255, 50, 255, 50);
    Gdiplus::Color COL_OFFLINE(255, 255, 50, 50);

    void AddRoundedRect(Gdiplus::GraphicsPath* path, Gdiplus::Rect rect, int r) {
        int d = r * 2;
        path->AddArc(rect.X, rect.Y, d, d, 180, 90);
        path->AddArc(rect.X + rect.Width - d, rect.Y, d, d, 270, 90);
        path->AddArc(rect.X + rect.Width - d, rect.Y + rect.Height - d, d, d, 0, 90);
        path->AddArc(rect.X, rect.Y + rect.Height - d, d, d, 90, 90);
        path->CloseFigure();
    }

    void DrawModernButton(Gdiplus::Graphics* g, Gdiplus::Rect rect, std::wstring text, bool sel, bool lock, bool showStatus = false) {
        g->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        Gdiplus::GraphicsPath path;
        AddRoundedRect(&path, rect, 10);

        Gdiplus::SolidBrush btnBrush(lock ? Gdiplus::Color(255, 30, 30, 35) : (sel ? COL_ACCENT : COL_BTN));
        g->FillPath(&btnBrush, &path);

        if (showStatus && sel) {
            Gdiplus::SolidBrush sBrush(GlobalState::IsGameOpened ? COL_ONLINE : COL_OFFLINE);
            g->FillEllipse(&sBrush, rect.X + 12, rect.Y + (rect.Height / 2) - 4, 8, 8);
        }

        Gdiplus::Font font(L"Segoe UI", 10, Gdiplus::FontStyleBold);
        Gdiplus::StringFormat format;
        format.SetAlignment(Gdiplus::StringAlignmentCenter);
        format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        Gdiplus::SolidBrush tBrush(lock ? Gdiplus::Color(255, 100, 100, 100) : Gdiplus::Color(255, 255, 255, 255));

        std::wstring finalStr = lock ? L"[LUX] " + text : text;
        g->DrawString(finalStr.c_str(), -1, &font, Gdiplus::RectF(rect.X, rect.Y, rect.Width, rect.Height), &format, &tBrush);
    }

    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        switch (msg) {
        case WM_CREATE:
            SetTimer(hwnd, 1, 1000, NULL);
            return 0;

        case WM_TIMER:
            if (wp == 1 && GlobalState::selectedGameId != -1) {
                for (const auto& g : GamesList) {
                    if (g.id == GlobalState::selectedGameId) {
                        bool running = (Injector::GetPidByName(g.processName.c_str()) != 0);
                        if (running != GlobalState::IsGameOpened) {
                            GlobalState::IsGameOpened = running;
                            InvalidateRect(hwnd, NULL, FALSE);
                        }
                        break;
                    }
                }
            }
            return 0;

        case WM_LBUTTONDOWN: {
            int mx = LOWORD(lp); int my = HIWORD(lp);
            for (const auto& btn : ActiveButtons) {
                if (mx >= btn.rect.X && mx <= btn.rect.X + btn.rect.Width &&
                    my >= btn.rect.Y && my <= btn.rect.Y + btn.rect.Height) {

                    if (!btn.isHax) {
                        GlobalState::selectedGameId = btn.gameId;
                        GlobalState::IsGameOpened = false;
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                    else {
                        // INJECT GOMB MEGNYOMVA
                        if (!GlobalState::IsGameOpened) {
                            MessageBoxW(hwnd, L"Inditsd el a jatekot elobb!", L"Hiba", MB_ICONWARNING);
                            return 0;
                        }

                        for (const auto& g : GamesList) {
                            if (g.id == GlobalState::selectedGameId) {
                                DWORD pid = Injector::GetPidByName(g.processName.c_str());
                                if (Injector::RunInjection(pid, g.dllPath.c_str()))
                                    MessageBoxW(hwnd, L"Sikeres Inject!", L"GG", MB_OK);
                                else
                                    MessageBoxW(hwnd, L"Inject Hiba! (Admin?)", L"Hiba", MB_ICONERROR);
                                break;
                            }
                        }
                    }
                }
            }
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc; GetClientRect(hwnd, &rc);
            HDC mdc = CreateCompatibleDC(hdc);
            HBITMAP mbmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
            SelectObject(mdc, mbmp);

            Gdiplus::Graphics g(mdc);
            g.Clear(COL_BG);
            ActiveButtons.clear();

            // Sidebar
            Gdiplus::SolidBrush sb(COL_SIDEBAR);
            g.FillRectangle(&sb, 0, 0, 260, rc.bottom);

            // Játék gombok
            for (int i = 0; i < GamesList.size(); i++) {
                Gdiplus::Rect r(20, 80 + (i * 55), 220, 45);
                DrawModernButton(&g, r, GamesList[i].name, (GlobalState::selectedGameId == GamesList[i].id), false, true);
                ActiveButtons.push_back({ r, GamesList[i].id, false });
            }

            // Hax gombok (csak ha van kiválasztott játék)
            if (GlobalState::selectedGameId != -1) {
                int count = 0;
                for (auto& h : HaxList) {
                    if (h.gameId == GlobalState::selectedGameId) {
                        Gdiplus::Rect r(260 + 30 + ((count % 2) * 310), 80 + ((count / 2) * 65), 290, 50);
                        DrawModernButton(&g, r, h.name, false, false);
                        ActiveButtons.push_back({ r, h.gameId, true });
                        count++;
                    }
                }
            }

            BitBlt(hdc, 0, 0, rc.right, rc.bottom, mdc, 0, 0, SRCCOPY);
            DeleteObject(mbmp); DeleteDC(mdc);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_DESTROY: PostQuitMessage(0); return 0;
        }
        return DefWindowProcW(hwnd, msg, wp, lp);
    }

    void Show() {
        Gdiplus::GdiplusStartupInput gsi; ULONG_PTR t;
        Gdiplus::GdiplusStartup(&t, &gsi, NULL);

        WNDCLASSEXW wc = { sizeof(wc), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, GetModuleHandle(NULL), NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, L"GG_UI", NULL };
        RegisterClassExW(&wc);

        HWND hwnd = CreateWindowExW(0, L"GG_UI", L"GG HAxs Multi-Loader", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 950, 650, NULL, NULL, wc.hInstance, NULL);

        MSG msg;
        while (GetMessageW(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
        Gdiplus::GdiplusShutdown(t);
    }
}