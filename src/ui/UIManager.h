#pragma once

#include <memory>
#include <string>

#include "ui/HUD.h"
#include "ui/Hotbar.h"
#include "ui/MainMenu.h"
#include "ui/PauseMenu.h"

struct GLFWwindow;
class Window;
struct Config;
class Inventory;
class PlayerController;
struct PlayerStatus;

class UIManager
{
public:
    UIManager() = default;
    ~UIManager();

    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;

    void Init(GLFWwindow* window);

    void BeginFrame(bool interactive);
    void EndFrame();

    MainMenu::Action RenderMainMenu(Window& window, Config& config, const std::string& configPath);
    PauseMenu::Action RenderPaused(Window& window, Config& config, const std::string& configPath,
                                    const PlayerStatus& status, const Inventory& inventory, int chunkCount);
    void RenderPlaying(const PlayerStatus& status, const Inventory& inventory, int chunkCount);

    MainMenu& GetMainMenu();
    PauseMenu& GetPauseMenu();

private:
    GLFWwindow* window = nullptr;
    bool initialized = false;

    HUD hud;
    Hotbar hotbar;
    MainMenu mainMenu;
    PauseMenu pauseMenu;
};
