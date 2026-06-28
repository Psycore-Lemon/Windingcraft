#pragma once

#include <memory>
#include <string>

#include "core/Window.h"
#include "core/Time.h"
#include "core/InputHandler.h"
#include "core/Config.h"

#include "game/BlockType.h"
#include "game/SaveData.h"

#include "scene/Camera.h"
#include "scene/Player.h"
#include "world/World.h"

class Shader;
class Mesh;
class Renderer;

class UIManager;
class HUD;
class MainMenu;
class PauseMenu;

class Engine
{
public:
    Engine();
    ~Engine();

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    bool Init(const std::string& configPath);
    void Run();

private:
    enum class State { Menu, Playing, Paused };

    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void SetState(State newState);

    State state = State::Menu;

    Window window;
    Config config;
    std::string configPath;

    Camera camera;
    Player player;
    std::unique_ptr<World> world;
    SaveData currentSave;
    void StartWorld(const SaveData& data);
    void SaveWorld();

    Time time;
    std::unique_ptr<InputHandler> input;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Shader> lineShader;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<UIManager> ui;
    std::unique_ptr<HUD> hud;
    std::unique_ptr<MainMenu> mainMenu;
    std::unique_ptr<PauseMenu> pauseMenu;

    bool escWasDown = false;
    bool breakWasDown = false;

    BlockType lookingAtBlock = BlockType::Air;
    glm::ivec3 lookingAtBlockPos = glm::ivec3(0);
    bool hasTarget = false;
    void UpdateLookTarget();
};
