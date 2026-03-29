struct Game {
    int id;
    std::wstring name;          // Megjelenített név
    std::wstring processName;   // Pl. L"javaw.exe"
    std::string dllPath;        // Pl. "C:\\cheats\\minecraft_hax.dll"
};

// Példa feltöltés:
std::vector<Game> GamesList = {
    { 1, L"Minecraft", L"javaw.exe", "C:\\GG\\mc_hook.dll" },
    { 2, L"Fortnite", L"FortniteClient-Win64-Shipping.exe", "C:\\GG\\fn_internal.dll" }
};