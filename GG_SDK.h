#pragma once
#include <windows.h>
#include <string>
#include <vector>

// Makró a könnyebb exportáláshoz
#ifdef GG_SDK_EXPORTS
#define GG_API __declspec(dllexport)
#else
#define GG_API __declspec(dllimport)
#endif

namespace GG_SDK {
    // Struktúra, amit a fejlesztõk is látnak
    struct GameInfo {
        int id;
        wchar_t name[256];
        wchar_t processName[256];
    };

    // Exportált függvények
    extern "C" {
        GG_API bool InitSDK();
        GG_API bool IsGameRunning(const wchar_t* processName);
        GG_API int GetActiveProcessCount();
        GG_API void GetDefaultGames(GameInfo* outGames, int* count);
    }
}