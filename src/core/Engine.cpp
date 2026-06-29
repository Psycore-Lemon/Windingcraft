#include "core/Engine.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>
#include <chrono>

#include "game/GameConfig.h"
#include "graphics/Shader.h"
#include "graphics/Renderer.h"
#include "graphics/WorldRenderer.h"
#include "graphics/TextureArray.h"
#include "graphics/PlayerRenderer.h"
#include "server/GameServer.h"
#include "server/HostedServer.h"
#include "net/ClientSession.h"
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
    colorShader = std::make_unique<Shader>("shaders/color-vertex.glsl", "shaders/color-fragment.glsl");
    renderer = std::make_unique<Renderer>(window.GetHandle());
    worldRenderer = std::make_unique<WorldRenderer>();
    playerRenderer = std::make_unique<PlayerRenderer>();

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
        latestCommand = controller.BuildCommand(*input);
        commandReady = true;

        if (!clientSession)
        {
            GameServer& gs = hostedServer
                ? hostedServer->GetGameServer() : *server;
            gs.QueueCommand(localPlayerId, latestCommand);
        }
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
    std::string worldDir = SaveManager::SaveDir() + "/" + data.name;

    if (host)
    {
        hostedServer = std::make_unique<HostedServer>();
        hostedServer->Start(data.seed, GameConfig::ChunkLoadRadius);
        hostedServer->SetSaveDir(worldDir);

        localPlayerId = hostedServer->GetGameServer().AddPlayer(data.playerPosition);
        hostedServer->RegisterPlayer(localPlayerId, "Host");

        renderWorld = &hostedServer->GetGameServer().GetWorld();
    }
    else
    {
        server = std::make_unique<GameServer>();
        server->Init(data.seed, GameConfig::ChunkLoadRadius);
        server->SetSaveDir(worldDir);

        localPlayerId = server->AddPlayer(data.playerPosition);
        renderWorld = &server->GetWorld();
    }

    accumulator = 0.0f;
    BuildLocalSnapshots();
    controller.GetCamera().position = localSnapshot.position +
        glm::vec3(0.0f, GameConfig::PlayerEyeHeight, 0.0f);
    worldRenderer->Sync(*renderWorld);
}

void Engine::ConnectToServer(const std::string& host, uint16_t port, const std::string& username)
{
    clientSession = std::make_unique<ClientSession>();

    if (!clientSession->Connect(host, port, username))
    {
        clientSession.reset();
        return;
    }

    for (int i = 0; i < 50 && !clientSession->IsAccepted(); ++i)
    {
        clientSession->Poll();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    if (!clientSession->IsAccepted())
    {
        clientSession.reset();
        return;
    }

    localPlayerId = clientSession->GetPlayerId();

    remoteWorld = std::make_unique<World>();
    renderWorld = remoteWorld.get();
    worldRenderer->Sync(*renderWorld);

    accumulator = 0.0f;
    controller.GetCamera().position = clientSession->GetSpawnPosition() +
        glm::vec3(0.0f, GameConfig::PlayerEyeHeight, 0.0f);

    SetState(State::Playing);
}

void Engine::LeaveGame()
{
    SaveWorld();
    clientSession.reset();
    remoteWorld.reset();
    hostedServer.reset();
    server.reset();
    renderWorld = nullptr;
    localPlayerId = -1;
    allSnapshots.clear();
    localSnapshot = {};
}

void Engine::SaveWorld()
{
    if (clientSession || localPlayerId < 0) return;

    currentSave.playerPosition = localSnapshot.position;
    SaveManager::Save(currentSave.name, currentSave);

    GameServer& gs = hostedServer
        ? hostedServer->GetGameServer() : *server;
    gs.SaveAll();
}

void Engine::BuildLocalSnapshots()
{
    GameServer& gs = hostedServer
        ? hostedServer->GetGameServer() : *server;

    allSnapshots.clear();
    for (const auto& [id, player] : gs.GetPlayers())
    {
        PlayerSnapshot snap;
        snap.playerId = id;
        snap.position = player->position;
        snap.grounded = player->IsGrounded();
        snap.flying = player->IsFlying();
        snap.vitals = player->GetVitals();
        snap.hasTarget = player->HasTarget();
        snap.lookingAtBlock = player->GetTargetBlock();
        snap.lookingAtBlockPos = player->GetTargetBlockPos();

        if (hostedServer)
            snap.username = hostedServer->GetPlayerUsername(id);

        allSnapshots.push_back(snap);

        if (id == localPlayerId)
        {
            localSnapshot = snap;
            localInventory = player->GetInventory();
        }
    }
}

void Engine::Update(float dt)
{
    if (state != State::Playing || localPlayerId < 0)
        return;

    accumulator += dt;
    if (accumulator > GameConfig::MaxAccumulator)
        accumulator = GameConfig::MaxAccumulator;

    if (clientSession)
    {
        while (accumulator >= GameConfig::TickInterval)
        {
            if (commandReady)
            {
                clientSession->SendCommand(latestCommand);
                commandReady = false;
            }
            accumulator -= GameConfig::TickInterval;
        }

        clientSession->Poll();
        clientSession->ApplyWorldChanges(*remoteWorld);
        clientSession->ApplyChunkData(*remoteWorld);

        localSnapshot = clientSession->GetLocalSnapshot();
        allSnapshots = clientSession->GetAllSnapshots();
    }
    else
    {
        while (accumulator >= GameConfig::TickInterval)
        {
            if (hostedServer)
                hostedServer->Tick(GameConfig::TickInterval);
            else if (server)
                server->Tick(GameConfig::TickInterval);

            accumulator -= GameConfig::TickInterval;
        }

        BuildLocalSnapshots();
    }

    controller.GetCamera().position = localSnapshot.position +
        glm::vec3(0.0f, GameConfig::PlayerEyeHeight, 0.0f);
    worldRenderer->Sync(*renderWorld);
}

void Engine::Render()
{
    renderer->BeginFrame();

    if (state != State::Menu && localPlayerId >= 0 && renderWorld)
    {
        const Camera& cam = controller.GetCamera();
        float aspectRatio = window.GetAspectRatio();

        blockTextures->Bind(0);
        worldRenderer->Render(*renderer, *shader, cam, aspectRatio);

        if (localSnapshot.hasTarget)
            renderer->DrawBlockHighlight(*lineShader, cam,
                localSnapshot.lookingAtBlockPos, aspectRatio);

        playerRenderer->Render(*renderer, *colorShader, cam, aspectRatio,
                               allSnapshots, localPlayerId);
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
        else if (action == MainMenu::Action::Connect)
        {
            const auto& info = ui->GetMainMenu().GetConnectionInfo();
            ConnectToServer(info.host, info.port, info.username);
        }
        else if (action == MainMenu::Action::Quit)
        {
            window.Close();
        }

        controller.GetCamera().SetFOV(config.fov);
    }
    else if (state == State::Paused)
    {
        PlayerStatus status;
        status.position = localSnapshot.position;
        status.grounded = localSnapshot.grounded;
        status.flying = localSnapshot.flying;
        status.hasTarget = localSnapshot.hasTarget;
        status.lookingAtBlock = localSnapshot.lookingAtBlock;
        status.vitals = localSnapshot.vitals;

        auto action = ui->RenderPaused(window, config, configPath,
                                        status, localInventory, chunkCount);

        if (action == PauseMenu::Action::Resume)
            SetState(State::Playing);
        else if (action == PauseMenu::Action::QuitToMenu)
        {
            LeaveGame();
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
        PlayerStatus status;
        status.position = localSnapshot.position;
        status.grounded = localSnapshot.grounded;
        status.flying = localSnapshot.flying;
        status.hasTarget = localSnapshot.hasTarget;
        status.lookingAtBlock = localSnapshot.lookingAtBlock;
        status.vitals = localSnapshot.vitals;

        ui->RenderPlaying(status, localInventory, chunkCount);

        const Camera& cam = controller.GetCamera();
        float ar = window.GetAspectRatio();
        playerRenderer->RenderNameTags(cam, ar, allSnapshots, localPlayerId);
    }

    ui->EndFrame();

    renderer->EndFrame();
}
