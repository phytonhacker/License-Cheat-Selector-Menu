#pragma once
#include <windows.h>
#include <string>
#include <vector>

namespace Injector {
    struct GameEntry {
        int id;
        const wchar_t* name;
        const wchar_t* exeName;
        const char* dllPath;
    };

    // Csak szólunk, hogy van ilyen lista, de nem itt töltjük fel
    extern std::vector<GameEntry> GamesList;

    // Csak a nevek (deklaráció), pontosvesszõvel a végén!
    DWORD GetPidByName(const wchar_t* processName);
    bool RunInjection(DWORD pid, const char* dllPath);
}