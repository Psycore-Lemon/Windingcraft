#include "core/Engine.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "game/GameConfig.h"
#include "graphics/Shader.h"
#include "graphics/Renderer.h"
#include "graphics/WorldRenderer.h"
#include "ui/UIManager.h"

Engine::Engine() = default;
Engine::~Engine() = default;

bool Engine::Init(const std::string& path)
{
    configPath = path;

    Config* loaded = ReadConfig(configPath);
    config = *loaded;
    delete loaded;

    if (!window.Init(config.windowWidth, config.windowHeight, "Windingcraft", config.fullscreen))
        return false;

    controller.AttachToWindow(window.GetHandle());

    input = std::make_unique<InputHandler>(window.GetHandle());
    shader = std::make_unique<Shader>("shaders/vertex-shader.glsl", "shaders/fragment-shader.glsl");
    lineShader = std::make_unique<Shader>("shaders/line-vertex.glsl", "shaders/line-fragment.glsl");
    renderer = std::make_unique<Renderer>(window.GetHandle());
    worldRenderer = std::make_unique<WorldRenderer>();

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

        ProcessInput(dt);
        Update(dt);
        Render();
    }
}

void Engine::ProcessInput(float dt)
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

    if (state == State::Playing)
        controller.ProcessInput(*input, *world, dt);
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

void Engine::StartWorld(const SaveData& data)
{
    currentSave = data;
    controller.Reset(data.playerPosition);
    world = std::make_unique<World>(data.seed, GameConfig::ChunkLoadRadius);

    std::string worldDir = SaveManager::SaveDir() + "/" + data.name;
    world->SetSaveDir(worldDir);

    world->Update(controller.GetPlayer().position);
    worldRenderer->Sync(*world);
}

void Engine::SaveWorld()
{
    currentSave.playerPosition = controller.GetPlayer().position;
    SaveManager::Save(currentSave.name, currentSave);
    world->SaveAll();
}

void Engine::Update(float dt)
{
    if (state != State::Playing)
        return;

    float clampedDt = (dt > GameConfig::MaxDeltaTime) ? GameConfig::MaxDeltaTime : dt;
    controller.Update(clampedDt, *world);
    worldRenderer->Sync(*world);
}

void Engine::Render()
{
    renderer->BeginFrame();

    if (state != State::Menu && world)
    {
        const Camera& cam = controller.GetCamera();
        float aspectRatio = window.GetAspectRatio();

        worldRenderer->Render(*renderer, *shader, cam, aspectRatio);

        if (controller.HasTarget())
            renderer->DrawBlockHighlight(*lineShader, cam, controller.GetLookingAtPos(), aspectRatio);
    }

    int chunkCount = worldRenderer ? worldRenderer->GetMeshCount() : 0;
    PlayerStatus status = controller.GetStatus();
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
    }
    else if (state == State::Paused)
    {
        auto action = ui->RenderPaused(window, config, configPath, status, controller.GetInventory(), chunkCount);

        if (action == PauseMenu::Action::Resume)
            SetState(State::Playing);
        else if (action == PauseMenu::Action::QuitToMenu)
        {
            SaveWorld();
            SetState(State::Menu);
        }
        else if (action == PauseMenu::Action::QuitToDesktop)
        {
            SaveWorld();
            window.Close();
        }
    }
    else
    {
        ui->RenderPlaying(status, controller.GetInventory(), chunkCount);
    }

    ui->EndFrame();

    renderer->EndFrame();
}
