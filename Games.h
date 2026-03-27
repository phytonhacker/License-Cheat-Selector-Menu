#pragma once
#include <string>
#include <vector>

struct Game {
    int id;
    std::wstring name;
    std::wstring processName; // Ezt keressük a feladatkezelõben
};

std::vector<Game> GamesList = {
    { 1, L"Fortnite", L"FortniteClient-Win64-Shipping.exe" },
    { 2, L"Minecraft", L"javaw.exe" }, // A Minecraft általában javaw.exe-ként fut
    { 3, L"Counter-Strike 2", L"cs2.exe" },
    { 4, L"Among Us", L"Among Us.exe" }
};