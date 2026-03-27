#include <windows.h>
#include <dwmapi.h>
#include <vector>
#include <string>
#include <gdiplus.h>
#include <algorithm>
#include "Inject.h"
// --- KONZOL KIIRTÁSA ---
// Ez a sor mondja meg a Visual Studiónak, hogy ne nyisson ablakot
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "Games.h"
#include "Haxs.h"
#include "Status.h"

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "dwmapi.lib")

namespace SelectorMenu {
    struct ClickableButton {
        Gdiplus::Rect rect;
        std::wstring name;
        int gameId;
        bool isHax;
    };

    std::vector<ClickableButton> ActiveButtons;
    int selectedGameId = -1;

    // Színek
    Gdiplus::Color COL_BG(255, 15, 15, 20);
    Gdiplus::Color COL_SIDEBAR(255, 25, 25, 30);
    Gdiplus::Color COL_ACCENT(255, 99, 102, 241);
    Gdiplus::Color COL_BTN(255, 45, 45, 55);
    Gdiplus::Color COL_LOCK(255, 25, 25, 30);

    void AddRoundedRect(Gdiplus::GraphicsPath* path, Gdiplus::Rect rect, int r) {
        int d = r * 2;
        path->AddArc(rect.X, rect.Y, d, d, 180, 90);
        path->AddArc(rect.X + rect.Width - d, rect.Y, d, d, 270, 90);
        path->AddArc(rect.X + rect.Width - d, rect.Y + rect.Height - d, d, d, 0, 90);
        path->AddArc(rect.X, rect.Y + rect.Height - d, d, d, 90, 90);
        path->CloseFigure();
    }

    // --- FIXÁLT RAJZOLÓ (L-VALUE BIZTOS) ---
    void DrawModernButton(Gdiplus::Graphics* g, Gdiplus::Rect rect, std::wstring text, bool sel, bool lock) {
        g->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

        Gdiplus::GraphicsPath path;
        AddRoundedRect(&path, rect, 10);

        Gdiplus::Color btnColor = lock ? COL_LOCK : (sel ? COL_ACCENT : COL_BTN);
        Gdiplus::SolidBrush buttonBrush(btnColor);
        g->FillPath(&buttonBrush, &path);

        // Szöveg objektumok kimentése változóba
        Gdiplus::Font font(L"Segoe UI", 10, Gdiplus::FontStyleBold);
        Gdiplus::StringFormat format;
        format.SetAlignment(Gdiplus::StringAlignmentCenter);
        format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

        Gdiplus::Color txtColor = lock ? Gdiplus::Color(255, 100, 100, 100) : Gdiplus::Color(255, 255, 255, 255);
        Gdiplus::SolidBrush textBrush(txtColor);

        std::wstring finalStr = lock ? L"[🔒]" + text : text;

        // RectF változó (L-VALUE!)
        Gdiplus::RectF boundRect(
            static_cast<Gdiplus::REAL>(rect.X),
            static_cast<Gdiplus::REAL>(rect.Y),
            static_cast<Gdiplus::REAL>(rect.Width),
            static_cast<Gdiplus::REAL>(rect.Height)
        );
        // A DrawString hívása a változókkal
        g->DrawString(finalStr.c_str(), -1, &font, boundRect, &format, &textBrush);
    }

    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        switch (msg) {
        case WM_LBUTTONDOWN: {
            int mx = LOWORD(lp);
            int my = HIWORD(lp);
            for (const auto& btn : ActiveButtons) {
                if (mx >= btn.rect.X && mx <= btn.rect.X + btn.rect.Width &&
                    my >= btn.rect.Y && my <= btn.rect.Y + btn.rect.Height) {

                    if (!btn.isHax && inject::is_game_Downloaded == false) {
                        selectedGameId = btn.gameId;
                        InvalidateRect(hwnd, NULL, FALSE);
                        return 0;
                    }
                    else {
                        // SZIGORÚ LOCK
                        std::string lic = Stat::License;
                        std::transform(lic.begin(), lic.end(), lic.begin(), ::tolower);

                        if (selectedGameId == 1 && lic != "premium") {
                            MessageBoxW(hwnd, L"you need premium License!", L"GG HAxs Security", MB_OK | MB_ICONERROR);
                            return 0;
                        }   
                        
                        MessageBoxW(hwnd, (L"Indítás: " + btn.name).c_str(), L"GG HAxs", MB_OK);
                        inject::Download_Game_Hax();
                        return 0;
                    }
                }
            }
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, 950, 650);
            SelectObject(memDC, memBitmap);

            Gdiplus::Graphics graphics(memDC);
            graphics.Clear(COL_BG);
            ActiveButtons.clear();

            // Sidebar
            Gdiplus::SolidBrush sideBrush(COL_SIDEBAR);
            graphics.FillRectangle(&sideBrush, 0, 0, 260, 650);

            for (size_t i = 0; i < GamesList.size(); i++) {
                Gdiplus::Rect r(20, 80 + (static_cast<int>(i) * 55), 220, 45);
                DrawModernButton(&graphics, r, GamesList[i].name, (selectedGameId == GamesList[i].id), false);
                ActiveButtons.push_back({ r, GamesList[i].name, GamesList[i].id, false });
            }

            // Hax terület
            if (selectedGameId != -1) {
                int count = 0;
                for (auto& h : HaxList) {
                    if (h.gameId == selectedGameId) {
                        int col = count % 2;
                        int row = count / 2;
                        Gdiplus::Rect r(260 + 30 + (col * 310), 80 + (row * 65), 290, 50);

                        std::string lic = Stat::License;
                        std::transform(lic.begin(), lic.end(), lic.begin(), ::tolower);
                        bool isLocked = (selectedGameId == 1 && lic != "premium");

                        DrawModernButton(&graphics, r, h.name, false, isLocked);
                        ActiveButtons.push_back({ r, h.name, h.gameId, true });
                        count++;
                    }
                }
            }

            BitBlt(hdc, 0, 0, 950, 650, memDC, 0, 0, SRCCOPY);
            DeleteObject(memBitmap);
            DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY: PostQuitMessage(0); break;
        default: return DefWindowProcW(hwnd, msg, wp, lp);
        }
        return 0;
    }

    void Show() {
        Gdiplus::GdiplusStartupInput gsi;
        ULONG_PTR token;
        Gdiplus::GdiplusStartup(&token, &gsi, NULL);
        WNDCLASSEXW wc = { sizeof(wc) };
        wc.lpfnWndProc = WndProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = L"ModernMenuV12";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClassExW(&wc);
        HWND hwnd = CreateWindowExW(WS_EX_LAYERED, L"ModernMenuV12", L"GG HAxs Loader", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 950, 650, NULL, NULL, wc.hInstance, NULL);
        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
        MSG msg;
        while (GetMessageW(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
        Gdiplus::GdiplusShutdown(token);
    }
}