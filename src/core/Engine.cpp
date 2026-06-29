#include "core/Engine.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "game/GameConfig.h"
#include "graphics/Shader.h"
#include "graphics/Renderer.h"
#include "graphics/WorldRenderer.h"
#include "graphics/TextureArray.h"
#include "server/GameServer.h"
#include "server/HostedServer.h"
#include "scene/Player.h"
#include "world/World.h"
#include "ui/UIManager.h"

Engine::Engine() = default;
Engine::~Engine() = default;

bool Engine::Init(const std::string& path)
{
    configPath = path;

    config = ReadConfig(configPath);

    if (!window.Init(config.windowWidth, config.windowHeight, "Windingcraft", config.fullscreen))
        return false;

    callbackData.camera = &controller.GetCamera();
    window.SetUserPointer(&callbackData);
    controller.GetCamera().SetFOV(config.fov);

    input = std::make_unique<InputHandler>(window.GetHandle());
    callbackData.input = input.get();

    controller.AttachToWindow(window.GetHandle());
    shader = std::make_unique<Shader>("shaders/vertex-shader.glsl", "shaders/fragment-shader.glsl");
    lineShader = std::make_unique<Shader>("shaders/line-vertex.glsl", "shaders/line-fragment.glsl");
    renderer = std::make_unique<Renderer>(window.GetHandle());
    worldRenderer = std::make_unique<WorldRenderer>();

    blockTextures = std::make_unique<TextureArray>(
        std::vector<std::string>{
            "assets/textures/stone.png",
            "assets/textures/dirt.png"
        }, 16);

    shader->Use();
    shader->SetInt("blockTextures", 0);

    ui = std::make_unique<UIManager>();
    ui->Init(window.GetHandle());

    SetState(State::Menu);

    return true;
}

void Engine::Run()
{
    while (!window.ShouldClose())
    {
        time.Update();
        float dt = static_cast<float>(time.DeltaTime());

        ProcessInput();
        Update(dt);
        Render();
    }
}

void Engine::ProcessInput()
{
    input->Update();

    if (state == State::Menu)
        return;

    bool escDown = input->IsActionDown(Action::Pause);
    if (escDown && !escWasDown)
    {
        if (state == State::Playing)
            SetState(State::Paused);
        else if (state == State::Paused)
            SetState(State::Playing);
    }
    escWasDown = escDown;

    if (state == State::Playing && localPlayerId >= 0)
    {
        PlayerCommand cmd = controller.BuildCommand(*input);
        GetActiveGameServer().QueueCommand(localPlayerId, cmd);
    }
}

void Engine::SetState(State newState)
{
    state = newState;

    bool captured = (state == State::Playing);
    window.SetCursorCaptured(captured);
    controller.GetCamera().SetActive(captured);

    if (state == State::Playing)
        ui->GetPauseMenu().Reset();
}

void Engine::StartWorld(const SaveData& data, bool host)
{
    currentSave = data;
    hosting = host;

    std::string worldDir = SaveManager::SaveDir() + "/" + data.name;

    if (hosting)
    {
        hostedServer = std::make_unique<HostedServer>();
        hostedServer->Start(data.seed, GameConfig::ChunkLoadRadius);
        hostedServer->SetSaveDir(worldDir);
        server = nullptr;

        localPlayerId = hostedServer->GetGameServer().AddPlayer(data.playerPosition);
    }
    else
    {
        server = std::make_unique<GameServer>();
        server->Init(data.seed, GameConfig::ChunkLoadRadius);
        server->SetSaveDir(worldDir);

        localPlayerId = server->AddPlayer(data.playerPosition);
    }

    accumulator = 0.0f;
    controller.SyncFromServer(GetLocalPlayer());
    worldRenderer->Sync(GetActiveWorld());
}

void Engine::SaveWorld()
{
    if (localPlayerId < 0) return;

    currentSave.playerPosition = GetLocalPlayer().position;
    SaveManager::Save(currentSave.name, currentSave);
    GetActiveGameServer().SaveAll();
}

GameServer& Engine::GetActiveGameServer()
{
    if (hostedServer) return hostedServer->GetGameServer();
    return *server;
}

World& Engine::GetActiveWorld()
{
    return GetActiveGameServer().GetWorld();
}

const Player& Engine::GetLocalPlayer()
{
    return GetActiveGameServer().GetPlayer(localPlayerId);
}

void Engine::Update(float dt)
{
    if (state != State::Playing || localPlayerId < 0)
        return;

    accumulator += dt;
    if (accumulator > GameConfig::MaxAccumulator)
        accumulator = GameConfig::MaxAccumulator;

    while (accumulator >= GameConfig::TickInterval)
    {
        if (hostedServer)
            hostedServer->Tick(GameConfig::TickInterval);
        else if (server)
            server->Tick(GameConfig::TickInterval);

        accumulator -= GameConfig::TickInterval;
    }

    controller.SyncFromServer(GetLocalPlayer());
    worldRenderer->Sync(GetActiveWorld());
}

void Engine::Render()
{
    renderer->BeginFrame();

    if (state != State::Menu && localPlayerId >= 0)
    {
        const Camera& cam = controller.GetCamera();
        float aspectRatio = window.GetAspectRatio();

        blockTextures->Bind(0);
        worldRenderer->Render(*renderer, *shader, cam, aspectRatio);

        const Player& player = GetLocalPlayer();
        if (player.HasTarget())
            renderer->DrawBlockHighlight(*lineShader, cam, player.GetTargetBlockPos(), aspectRatio);
    }

    int chunkCount = worldRenderer ? worldRenderer->GetMeshCount() : 0;
    bool interactive = (state != State::Playing);
    ui->BeginFrame(interactive);

    if (state == State::Menu)
    {
        auto action = ui->RenderMainMenu(window, config, configPath);

        if (action == MainMenu::Action::Play)
        {
            StartWorld(ui->GetMainMenu().GetWorldData());
            SetState(State::Playing);
        }
        else if (action == MainMenu::Action::Quit)
        {
            window.Close();
        }

        controller.GetCamera().SetFOV(config.fov);
    }
    else if (state == State::Paused)
    {
        const Player& player = GetLocalPlayer();
        PlayerStatus status = controller.GetStatus(player);

        auto action = ui->RenderPaused(window, config, configPath, status, player.GetInventory(), chunkCount);

        if (action == PauseMenu::Action::Resume)
            SetState(State::Playing);
        else if (action == PauseMenu::Action::QuitToMenu)
        {
            SaveWorld();
            hostedServer.reset();
            server.reset();
            localPlayerId = -1;
            hosting = false;
            SetState(State::Menu);
        }
        else if (action == PauseMenu::Action::QuitToDesktop)
        {
            SaveWorld();
            window.Close();
        }

        controller.GetCamera().SetFOV(config.fov);
    }
    else
    {
        const Player& player = GetLocalPlayer();
        PlayerStatus status = controller.GetStatus(player);
        ui->RenderPlaying(status, player.GetInventory(), chunkCount);
    }

    ui->EndFrame();

    renderer->EndFrame();
}
