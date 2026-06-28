#pragma once

#include <string>

#include "game/SaveData.h"

class Window;
struct Config;

class MainMenu
{
public:
    enum class Action { None, Play, Quit };

    Action Render(Window& window, Config& config, const std::string& configPath);

    const SaveData& GetWorldData() const;

private:
    enum class Page { Main, NewWorld, LoadWorld, Options };
    Page currentPage = Page::Main;

    char nameInput[64] = "My World";
    char seedInput[32] = "";
    SaveData worldData;

    std::vector<std::string> saveList;
};
