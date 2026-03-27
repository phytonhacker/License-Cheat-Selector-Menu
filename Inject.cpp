#include <windows.h>
#include <commctrl.h>
#include "Inject.h"
#include <urlmon.h>
#include <iostream>

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "comctl32.lib")

#include <windows.h>
#include <urlmon.h>
#include <iostream>

#pragma comment(lib, "urlmon.lib")

bool DownloadDLL(const std::wstring& url, const std::wstring& savePath) {
    // URLDownloadToFileW paraméterei:
    // 1. Caller (NULL)
    // 2. URL címe
    // 3. Hova mentse a gépen
    // 4. Fenntartott (0)
    // 5. Status callback (NULL)

    HRESULT hr = URLDownloadToFileW(NULL, url.c_str(), savePath.c_str(), 0, NULL);

    if (SUCCEEDED(hr)) {
        return true;
    }
    else {
        return false;
    }
}

void Download_external() {
    std::wstring durl = L"https://cdn.discordapp.com/attachments/1461830676574896170/1487159517857845318/teszt.dll?ex=69c820bb&is=69c6cf3b&hm=90f3a81cfa357c78cb9c748e953e482e9016aab100dd08216be9b57e1d662b1b&";
    std::wstring path = L"C:\\teszt.dll";
    inject::is_game_Downloaded = true;
    std::wcout << L"Download DLL..." << std::endl;
    if (DownloadDLL(durl, path)) {
        MessageBoxW(NULL, L"Download Successful!", L"GG HAxs", MB_OK | MB_ICONINFORMATION);
    }
    else {
        MessageBoxW(NULL, L"Download Failed! Check your internet.", L"Error", MB_OK | MB_ICONERROR);
    }
}
void inject::Download_Game_Hax() {
    if (inject::is_game_Downloaded) {
        TASKDIALOGCONFIG config = { 0 };
        config.cbSize = sizeof(config);
        config.hwndParent = NULL;
        config.dwFlags = TDF_USE_COMMAND_LINKS; // Modern kinézet
        config.pszWindowTitle = L"Update Check";
        config.pszMainInstruction = L"Hax already exists!";
        config.pszContent = L"The files are already on your PC. What would you like to do?";

        // Saját gombok definiálása
        const TASKDIALOG_BUTTON buttons[] = {
            { IDYES, L"Check for Updates\nDownload the latest version from the server." },
            { IDNO,  L"Launch Anyway\nI don't care about updates, just start it." }
        };
        config.pButtons = buttons;
        config.cButtons = 2;

        int result = 0;
        TaskDialogIndirect(&config, &result, NULL, NULL);

        if (result == IDYES) {
            // Update kód
        }
        else {
            
        }
    }
    else {
        Download_external();
    }
}