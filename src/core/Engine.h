#pragma once

#include <memory>
#include <string>
#include <vector>

#include "core/Window.h"
#include "core/Time.h"
#include "core/InputHandler.h"
#include "core/Config.h"
#include "core/CallbackData.h"

#include "game/SaveData.h"
#include "game/PlayerCommand.h"
#include "game/Inventory.h"
#include "scene/PlayerController.h"
#include "net/PacketSerializer.h"

class Shader;
class Renderer;
class WorldRenderer;
class TextureArray;
class PlayerRenderer;
class UIManager;
class GameServer;
class HostedServer;
class ClientSession;
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
    void ConnectToServer(const std::string& host, uint16_t port, const std::string& username);
    void LeaveGame();
    void SaveWorld();

    void BuildLocalSnapshots();

    State state = State::Menu;

    Window window;
    Config config;
    std::string configPath;

    // Client-side
    PlayerController controller;
    PlayerSnapshot localSnapshot;
    glm::vec3 smoothedPosition{0.0f};
    std::vector<PlayerSnapshot> allSnapshots;
    Inventory localInventory;
    World* renderWorld = nullptr;
    int localPlayerId = -1;

    // Server (exactly one of these is active, or none in remote mode)
    std::unique_ptr<GameServer> server;
    std::unique_ptr<HostedServer> hostedServer;

    // Remote client (active when connected to a remote server)
    std::unique_ptr<ClientSession> clientSession;
    std::unique_ptr<World> remoteWorld;

    SaveData currentSave;

    Time time;
    std::unique_ptr<InputHandler> input;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Shader> lineShader;
    std::unique_ptr<Shader> colorShader;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<WorldRenderer> worldRenderer;
    std::unique_ptr<TextureArray> blockTextures;
    std::unique_ptr<PlayerRenderer> playerRenderer;
    std::unique_ptr<UIManager> ui;

    CallbackData callbackData;
    bool escWasDown = false;
    float accumulator = 0.0f;

    PlayerCommand latestCommand;
    bool commandReady = false;
};
