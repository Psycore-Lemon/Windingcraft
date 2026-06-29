#pragma once

#include <memory>
#include <string>

#include "core/Window.h"
#include "core/Time.h"
#include "core/InputHandler.h"
#include "core/Config.h"
#include "core/CallbackData.h"

#include "game/SaveData.h"
#include "scene/PlayerController.h"

class Shader;
class Renderer;
class WorldRenderer;
class TextureArray;
class UIManager;
class GameServer;
class HostedServer;
class World;
class Player;

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

    void ProcessInput();
    void Update(float dt);
    void Render();
    void SetState(State newState);
    void StartWorld(const SaveData& data, bool host = true);
    void SaveWorld();

    GameServer& GetActiveGameServer();
    World& GetActiveWorld();
    const Player& GetLocalPlayer();

    State state = State::Menu;

    Window window;
    Config config;
    std::string configPath;

    PlayerController controller;
    std::unique_ptr<GameServer> server;
    std::unique_ptr<HostedServer> hostedServer;
    int localPlayerId = -1;
    bool hosting = false;
    SaveData currentSave;

    Time time;
    std::unique_ptr<InputHandler> input;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Shader> lineShader;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<WorldRenderer> worldRenderer;
    std::unique_ptr<TextureArray> blockTextures;
    std::unique_ptr<UIManager> ui;

    CallbackData callbackData;
    bool escWasDown = false;
    float accumulator = 0.0f;
};
