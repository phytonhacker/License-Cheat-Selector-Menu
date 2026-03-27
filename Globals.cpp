#include "Globals.h"

// színek
const COLORREF CLR_BG = RGB(15, 15, 20);
const COLORREF CLR_SURFACE = RGB(35, 35, 52);

// brush
HBRUSH hBrushBg = nullptr;
HBRUSH hBrushSurface = nullptr;

// fontok
HFONT hFontUI = nullptr;
HFONT hFontTitle = nullptr;
HFONT hFontAscii = nullptr;
HFONT hFontBtn = nullptr;

// blur
bool g_blurEnabled = false;