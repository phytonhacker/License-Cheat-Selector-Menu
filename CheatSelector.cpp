#include <windows.h>
#include <dwmapi.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <gdiplus.h>
#include <string>
#include <algorithm>

#include "Status.h"
#include "Globals.h"
#include "SelectorMenu.h"

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "uxtheme.lib")

// --- KONZOL KIIRTÁSA ---
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define ID_BTN 101
#define ID_TXT 102

// ═══════════════════════════════════════════════════════════
//  Undocumented SWCA struktúrák
// ═══════════════════════════════════════════════════════════
struct ACCENT_POLICY {
    DWORD AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
};
struct WINDOWCOMPOSITIONATTRIBDATA {
    DWORD dwAttrib;
    PVOID pvData;
    SIZE_T cbData;
};
typedef BOOL(WINAPI* pfnSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

// ═══════════════════════════════════════════════════════════
//  Konstansok & Globálisok
// ═══════════════════════════════════════════════════════════
constexpr COLORREF CLR_ACCENT = RGB(99, 102, 241);
constexpr COLORREF CLR_ACCENT_H = RGB(129, 132, 255);
constexpr COLORREF CLR_TEXT = RGB(240, 240, 255);
constexpr COLORREF CLR_RED = RGB(220, 50, 50);

const wchar_t* ASCII_ART[] = {
    L"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀",
    L"⠀⠀⠀⡠⢺⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢢⠀",
    L"⠀⠀⣰⠁⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠀⢳⠀⠀",
    L"⠀⢠⡇⠀⢸⠀⠀⠀⠀⢀⣀⣀⣀⣀⣀⣀⣀⠀⠀⠀⠀⠈⠀⠀⡇⠀⠀",
    L"⠀⢸⡄⠀⠀⠢⣠⣴⡋⠉⠁⠀⠀⠀⠀⠀⠈⠻⣶⣄⡠⠃⠀⠀⡇⠀⠀",
    L"⡀⠈⣧⠀⠀⠀⠈⡇⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⡏⠀⠀⠀⢠⠇⠀⡀",
    L"⣷⡀⠘⣆⠀⠀⠀⣿⠀⠀⠀⠀⠀⢀⣀⣠⡤⢾⣿⡁⠀⠀⢠⡟⠀⣴⡇",
    L"⡏⢻⣦⡘⣷⣦⠼⠃⠀⠀⠀⠀⠀⠀⠀⢀⠀⠀⠻⢷⣤⣴⡏⣠⡾⠃⠃",
    L"⢳⠀⠙⢿⣿⡏⠀⠀⠀⡰⠃⠀⠀⠀⠀⠀⠣⡀⠀⠀⠀⢹⣿⠟⠀⢰⠀",
    L"⠈⢧⡀⢸⣿⣿⣀⡀⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⣀⠀⠈⣿⠀⢀⠌⠀",
    L"⠀⠀⠙⢾⣿⣿⣿⣿⡗⢄⡀⠀⡀⠀⡀⠀⡀⠔⡿⠀⠀⠀⣿⠔⠁⠀",
    L"⠀⠀⠀⠀⢿⣿⡋⠉⠙⠒⠉⠙⡇⢀⡟⠉⠑⠊⠁⠀⠀⢠⠇⠀⠀",
    L"⠀⠀⠀⠀⠈⠻⡇⠀⠀⠀⢀⠀⠃⢸⣷⣀⠀⠀⠀⢀⡴⠃⠀⠀",
    L"⠀⠀⠀⠀⠀⠀⣿⣶⣶⣖⠛⠦⠀⠈⡿⠟⢑⣶⣶⣾⠇⠀⠀",
    L"⠀⠀⠀⠀⠀⠀⢻⣯⠉⢩⠧⠄⠓⠒⠁⣔⢵⠈⣡⣿⠀⠀",
    L"⠀⠀⠀⠀⠀⠀⠈⢿⣧⠘⢜⣉⣁⣈⣉⣏⠄⢰⣿⠃⠀⠀",
    L"⠀⠀⠀⠀⠀⠀⠀⠈⢿⡀⠀⠐⠒⠒⠒⠀⠀⣼⠃",
    L"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠑⢤⣀⣀⣀⣀⣠⠜⠁⠀",
    L"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⣉⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
};
constexpr int ASCII_LINES = 19;

// ═══════════════════════════════════════════════════════════
//  Rendszer funkciók
// ═══════════════════════════════════════════════════════════
bool IsWindowsTransparencyEnabled() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD val = 1, sz = sizeof(val);
        RegQueryValueExW(hKey, L"EnableTransparency", NULL, NULL, (BYTE*)&val, &sz);
        RegCloseKey(hKey);
        return val != 0;
    }
    return false;
}

bool EnableAcrylicBlur(HWND hwnd) {
    if (!IsWindowsTransparencyEnabled()) return false;
    HMODULE hUser = GetModuleHandleW(L"user32.dll");
    if (!hUser) return false;
    auto swca = (pfnSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
    if (!swca) return false;

    ACCENT_POLICY accent = { 4, 2, 0xAA140F0F, 0 }; // Acrylic
    WINDOWCOMPOSITIONATTRIBDATA data = { 19, &accent, sizeof(accent) };
    if (swca(hwnd, &data)) return true;

    accent.AccentState = 3; // Fallback blur
    return swca(hwnd, &data);
}

// ═══════════════════════════════════════════════════════════
//  GDI+ Rajzolás (FIXÁLT l-value hiba ellen)
// ═══════════════════════════════════════════════════════════
void AddRoundedRectPath(Gdiplus::GraphicsPath& p, int x, int y, int w, int h, int r) {
    p.AddArc(x, y, r * 2, r * 2, 180, 90);
    p.AddArc(x + w - r * 2, y, r * 2, r * 2, 270, 90);
    p.AddArc(x + w - r * 2, y + h - r * 2, r * 2, r * 2, 0, 90);
    p.AddArc(x, y + h - r * 2, r * 2, r * 2, 90, 90);
    p.CloseFigure();
}

void DrawLoginButton(HWND hBtn, HDC hdc, bool pressed) {
    RECT rc; GetClientRect(hBtn, &rc);
    int w = rc.right, h = rc.bottom;

    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, w, h);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, bmp);

    {
        Gdiplus::Graphics g(memDC);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

        Gdiplus::SolidBrush bg(Gdiplus::Color(255, GetRValue(CLR_BG), GetGValue(CLR_BG), GetBValue(CLR_BG)));
        g.FillRectangle(&bg, 0, 0, w, h);

        COLORREF c = pressed ? CLR_ACCENT_H : CLR_ACCENT;
        Gdiplus::SolidBrush fill(Gdiplus::Color(255, GetRValue(c), GetGValue(c), GetBValue(c)));
        Gdiplus::GraphicsPath path;
        AddRoundedRectPath(path, 0, 0, w, h, 9);
        g.FillPath(&fill, &path);
    }

    WCHAR txt[128] = {};
    GetWindowTextW(hBtn, txt, 128);
    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, RGB(255, 255, 255));
    HFONT oldFont = (HFONT)SelectObject(memDC, hFontBtn);
    DrawTextW(memDC, txt, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(memDC, oldFont);

    BitBlt(hdc, 0, 0, w, h, memDC, 0, 0, SRCCOPY);
    SelectObject(memDC, oldBmp);
    DeleteObject(bmp);
    DeleteDC(memDC);
}

// ═══════════════════════════════════════════════════════════
//  WndProc & Main
// ═══════════════════════════════════════════════════════════
LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, UINT_PTR, DWORD_PTR) {
    if (msg == WM_CHAR && (wp == VK_RETURN || wp == '\n')) return 0;
    if (msg == WM_KEYDOWN && wp == VK_RETURN) return 0;
    return DefSubclassProc(hwnd, msg, wp, lp);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        BOOL dark = TRUE;
        DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
        DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_ROUND;
        DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));

        g_blurEnabled = EnableAcrylicBlur(hwnd);
        hBrushBg = CreateSolidBrush(CLR_BG);
        hBrushSurface = CreateSolidBrush(CLR_SURFACE);

        hFontUI = CreateFontW(17, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        hFontTitle = CreateFontW(24, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        hFontAscii = CreateFontW(14, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, FIXED_PITCH, L"Consolas");
        hFontBtn = CreateFontW(15, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");

        HWND hTxt = CreateWindowExW(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOHSCROLL, 30, 108, 220, 36, hwnd, (HMENU)ID_TXT, NULL, NULL);
        SendMessageW(hTxt, WM_SETFONT, (WPARAM)hFontUI, TRUE);
        SetWindowTheme(hTxt, L"", L"");

        RECT rcE; GetClientRect(hTxt, &rcE);
        HDC htmp = GetDC(hTxt);
        HFONT of = (HFONT)SelectObject(htmp, hFontUI);
        TEXTMETRICW tm; GetTextMetricsW(htmp, &tm);
        SelectObject(htmp, of); ReleaseDC(hTxt, htmp);
        int pad = (rcE.bottom - tm.tmHeight) / 2;
        RECT fr = { 10, pad, rcE.right - 10, rcE.bottom };
        SendMessageW(hTxt, EM_SETRECT, 0, (LPARAM)&fr);
        SetWindowSubclass(hTxt, EditSubclassProc, 0, 0);

        CreateWindowExW(0, L"BUTTON", L"Authenticate License", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 30, 158, 220, 38, hwnd, (HMENU)ID_BTN, NULL, NULL);
        break;
    }
    case WM_DRAWITEM: {
        auto* dis = (DRAWITEMSTRUCT*)lp;
        if (dis->CtlID == ID_BTN) DrawLoginButton(dis->hwndItem, dis->hDC, (dis->itemState & ODS_SELECTED) != 0);
        return TRUE;
    }
    case WM_CTLCOLOREDIT: {
        HDC hdc = (HDC)wp;
        SetTextColor(hdc, CLR_TEXT);
        SetBkColor(hdc, CLR_SURFACE);
        return (LRESULT)hBrushSurface;
    }
    case WM_ERASEBKGND: {
        if (g_blurEnabled) return TRUE;
        RECT rc; GetClientRect(hwnd, &rc);
        FillRect((HDC)wp, &rc, hBrushBg);
        return TRUE;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        SetBkMode(hdc, TRANSPARENT);
        SelectObject(hdc, hFontTitle);
        SetTextColor(hdc, CLR_TEXT);
        RECT rcLabel = { 30, 66, 280, 104 };
        DrawTextW(hdc, L"GG HAxs", -1, &rcLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hdc, hFontAscii);
        SetTextColor(hdc, CLR_RED);
        RECT rcClient; GetClientRect(hwnd, &rcClient);
        int ax = rcClient.right / 2 + 20;
        int ay = 18;
        for (int i = 0; i < ASCII_LINES; i++) {
            RECT line = { ax, ay + i * 18, rcClient.right - 10, ay + (i + 1) * 18 };
            DrawTextW(hdc, ASCII_ART[i], -1, &line, DT_LEFT | DT_SINGLELINE);
        }
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wp) == ID_BTN) {
            WCHAR buf[256] = {};
            GetDlgItemTextW(hwnd, ID_TXT, buf, 256);
            if (buf[0] == L'\0') {
                MessageBoxW(hwnd, L"Add a valid License key", L"Error", MB_OK | MB_ICONWARNING);
                return 0;
            }
            // --- LICENC ELLENŐRZÉS ---
            std::wstring wLicense(buf);
            std::string sLicense(wLicense.begin(), wLicense.end());
            std::transform(sLicense.begin(), sLicense.end(), sLicense.begin(), ::tolower);

            if (sLicense == "free" || sLicense == "premium") {
                MessageBoxW(hwnd, L"Success! Welcome.", L"GG HAxs", MB_OK | MB_ICONINFORMATION);
                Stat::License = (sLicense == "premium") ? "premium" : "free";
                SelectorMenu::Show();
                ShowWindow(hwnd, SW_HIDE);
            }
            else {
                MessageBoxW(hwnd, L"Invalid License key", L"Error", MB_OK | MB_ICONERROR);
            }
        }
        break;
    case WM_DESTROY: PostQuitMessage(0); break;
    default: return DefWindowProcW(hwnd, msg, wp, lp);
    }
    return 0;
}

int main() {
    Gdiplus::GdiplusStartupInput gsi;
    ULONG_PTR tok;
    Gdiplus::GdiplusStartup(&tok, &gsi, NULL);

    WNDCLASSEXW wc = { sizeof(wc) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = L"ModernAppV2";
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(CLR_BG);
    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(0, L"ModernAppV2", L"GG HAxs Login", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 720, 420, NULL, NULL, wc.hInstance, NULL);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    Gdiplus::GdiplusShutdown(tok);
    return (int)msg.wParam;
}