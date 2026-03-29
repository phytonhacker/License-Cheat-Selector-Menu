#include "GG_SDK.h"
#include <tlhelp32.h>

#define GG_SDK_EXPORTS // Ez jelzi, hogy most exportálunk

namespace GG_SDK {

    bool InitSDK() {
        // Itt inicializálhatsz bármit (pl. logolás, szerver kapcsolat)
        return true;
    }

    bool IsGameRunning(const wchar_t* processName) {
        bool exists = false;
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32W pe;
            pe.dwSize = sizeof(PROCESSENTRY32W);
            if (Process32FirstW(hSnapshot, &pe)) {
                do {
                    if (wcscmp(pe.szExeFile, processName) == 0) {
                        exists = true;
                        break;
                    }
                } while (Process32NextW(hSnapshot, &pe));
            }
            CloseHandle(hSnapshot);
        }
        return exists;
    }

    // Példa: A fejlesztõ lekérheti az elõre beépített játéklistát
    void GetDefaultGames(GameInfo* outGames, int* count) {
        static GameInfo internalList[] = {
            { 1, L"Fortnite", L"FortniteClient-Win64-Shipping.exe" },
            { 2, L"Minecraft", L"javaw.exe" }
        };

        if (outGames == nullptr) {
            *count = 2; // Csak a darabszámot adjuk vissza
            return;
        }

        for (int i = 0; i < 2; i++) {
            outGames[i] = internalList[i];
        }
    }
}