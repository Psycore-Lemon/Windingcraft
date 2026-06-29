#pragma once

#include <string>
#include <cstdint>

#include "game/SaveData.h"

class Window;
struct Config;

struct ConnectionInfo
{
    std::string host = "127.0.0.1";
    uint16_t port = 7777;
    std::string username = "Player";
};

class MainMenu
{
public:
    enum class Action { None, Play, Connect, Quit };

    Action Render(Window& window, Config& config, const std::string& configPath);

    const SaveData& GetWorldData() const;
    const ConnectionInfo& GetConnectionInfo() const;

private:
    enum class Page { Main, Singleplayer, NewWorld, Multiplayer, Options };
    Page currentPage = Page::Main;

    char nameInput[64] = "My World";
    char seedInput[32] = "";
    SaveData worldData;

    char hostInput[64] = "127.0.0.1";
    char portInput[8] = "7777";
    char usernameInput[32] = "Player";
    ConnectionInfo connectionInfo;

    std::vector<std::string> saveList;
    std::string pendingDelete;

    void RefreshSaveList();
};
