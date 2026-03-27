#include "Inject.h"
#include <windows.h>
#include <tlhelp32.h>
#include <string>

namespace inject {
	bool is_game_Downloaded = false;
	bool IsInjected = false;
	bool IsGameOpened = false;

    // Függvény, ami megnézi, fut-e egy adott .exe
    bool IsProcessRunning(const std::wstring& processName) {
        bool exists = false;
        // Készítünk egy pillanatképet az összes futó folyamatról
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (hSnapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32W pe;
            pe.dwSize = sizeof(PROCESSENTRY32W);

            // Elindulunk az elsõ folyamattól
            if (Process32FirstW(hSnapshot, &pe)) {
                do {
                    // Összehasonlítjuk a folyamat nevét azzal, amit keresünk
                    if (processName == pe.szExeFile) {
                        exists = true;
                        break;
                    }
                } while (Process32NextW(hSnapshot, &pe)); // Megyünk a következõre
            }
            CloseHandle(hSnapshot); // Be zárjuk a "fényképet"
        }
        return exists;
    }
}