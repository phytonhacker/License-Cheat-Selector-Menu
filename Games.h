#pragma once
#include <string>
#include <vector>

struct Game {
    std::wstring name;
    int id; // azonosító
};

// Példa játékok
extern std::vector<Game> GamesList = {
    { L"Fortnite", 1 },
    { L"Minecraft", 2 },
    { L"CS:GO", 3 },
    { L"Apex Legends", 4 },
};