#pragma once

#include <string>

class Window;
struct Config;

class PauseMenu
{
public:
    enum class Action { None, Resume, QuitToMenu, QuitToDesktop };

    Action Render(Window& window, Config& config, const std::string& configPath);
    void Reset();

private:
    enum class Page { Main, Options };
    Page currentPage = Page::Main;
};
