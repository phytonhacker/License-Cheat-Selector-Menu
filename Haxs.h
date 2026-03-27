#pragma once
#include <string>
#include <vector>
#include <map>

// hack struktúra
struct Hax {
    std::wstring name;
    int gameId; // melyik játékhoz tartozik
};

// példa hackek
extern std::vector<Hax> HaxList = {
    { L"XeoHack", 1 },
    { L"Y_Hacks.com", 2},
};