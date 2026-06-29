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
#include "game/Inventory.h"
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

    colorShader = std::make_unique<Shader>("shaders/color-vertex.glsl", "shaders/color-fragment.glsl");
    playerRenderer = std::make_unique<PlayerRenderer>();

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

        if (IsRemoteClient())
            clientSession->SendCommand(cmd);
        else
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

void Engine::ConnectToServer(const std::string& host, uint16_t port, const std::string& username)
{
    clientSession = std::make_unique<ClientSession>();

    if (!clientSession->Connect(host, port, username))
    {
        clientSession.reset();
        return;
    }

    // Wait briefly for accept packet
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
    worldRenderer->Sync(*remoteWorld);

    accumulator = 0.0f;
    controller.GetCamera().position = clientSession->GetSpawnPosition() +
        glm::vec3(0.0f, GameConfig::PlayerEyeHeight, 0.0f);

    SetState(State::Playing);
}

void Engine::SaveWorld()
{
    if (IsRemoteClient() || localPlayerId < 0) return;

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
    if (remoteWorld) return *remoteWorld;
    return GetActiveGameServer().GetWorld();
}

const Player& Engine::GetLocalPlayer()
{
    return GetActiveGameServer().GetPlayer(localPlayerId);
}

bool Engine::IsRemoteClient() const
{
    return clientSession != nullptr;
}

void Engine::Update(float dt)
{
    if (state != State::Playing || localPlayerId < 0)
        return;

    if (IsRemoteClient())
    {
        clientSession->Poll();
        clientSession->ApplyWorldChanges(*remoteWorld);
        clientSession->ApplyChunkData(*remoteWorld);

        const auto& snap = clientSession->GetLocalSnapshot();
        controller.GetCamera().position = snap.position +
            glm::vec3(0.0f, GameConfig::PlayerEyeHeight, 0.0f);

        worldRenderer->Sync(*remoteWorld);
    }
    else
    {
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

        if (!IsRemoteClient())
        {
            const Player& player = GetLocalPlayer();
            if (player.HasTarget())
                renderer->DrawBlockHighlight(*lineShader, cam, player.GetTargetBlockPos(), aspectRatio);
        }
        else
        {
            const auto& snap = clientSession->GetLocalSnapshot();
            if (snap.hasTarget)
                renderer->DrawBlockHighlight(*lineShader, cam, snap.lookingAtBlockPos, aspectRatio);
        }

        if (IsRemoteClient())
        {
            playerRenderer->Render(*renderer, *colorShader, cam, aspectRatio,
                                   clientSession->GetAllSnapshots(), localPlayerId);
        }
        else if (hostedServer)
        {
            std::vector<PlayerSnapshot> snapshots;
            for (const auto& [id, player] : hostedServer->GetGameServer().GetPlayers())
            {
                PlayerSnapshot snap;
                snap.playerId = id;
                snap.position = player->position;
                snapshots.push_back(snap);
            }
            playerRenderer->Render(*renderer, *colorShader, cam, aspectRatio,
                                   snapshots, localPlayerId);
        }
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
        const Inventory* inv = nullptr;

        if (IsRemoteClient())
        {
            const auto& snap = clientSession->GetLocalSnapshot();
            status.position = snap.position;
            status.grounded = snap.grounded;
            status.flying = snap.flying;
            status.hasTarget = snap.hasTarget;
            status.lookingAtBlock = snap.lookingAtBlock;
            status.vitals = snap.vitals;
        }
        else
        {
            const Player& player = GetLocalPlayer();
            status = controller.GetStatus(player);
            inv = &player.GetInventory();
        }

        static Inventory emptyInv;
        auto action = ui->RenderPaused(window, config, configPath, status,
                                        inv ? *inv : emptyInv, chunkCount);

        if (action == PauseMenu::Action::Resume)
            SetState(State::Playing);
        else if (action == PauseMenu::Action::QuitToMenu)
        {
            SaveWorld();
            clientSession.reset();
            remoteWorld.reset();
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
        PlayerStatus status;
        const Inventory* inv = nullptr;

        if (IsRemoteClient())
        {
            const auto& snap = clientSession->GetLocalSnapshot();
            status.position = snap.position;
            status.grounded = snap.grounded;
            status.flying = snap.flying;
            status.hasTarget = snap.hasTarget;
            status.lookingAtBlock = snap.lookingAtBlock;
            status.vitals = snap.vitals;
        }
        else
        {
            const Player& player = GetLocalPlayer();
            status = controller.GetStatus(player);
            inv = &player.GetInventory();
        }

        static Inventory emptyInv;
        ui->RenderPlaying(status, inv ? *inv : emptyInv, chunkCount);
    }

    ui->EndFrame();

    renderer->EndFrame();
}
