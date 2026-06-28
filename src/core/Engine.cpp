#include "core/Engine.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/Shader.h"
#include "graphics/Mesh.h"
#include "graphics/Renderer.h"

#include "ui/UIManager.h"
#include "ui/HUD.h"
#include "ui/MainMenu.h"
#include "ui/PauseMenu.h"

Engine::Engine()
    : player(glm::vec3(0.0f, 20.0f, 0.0f))
{
}

Engine::~Engine() = default;

bool Engine::Init(const std::string& path)
{
    configPath = path;

    Config* loaded = ReadConfig(configPath);
    config = *loaded;
    delete loaded;

    if (!window.Init(config.windowWidth, config.windowHeight, "Windingcraft", config.fullscreen))
        return false;

    camera.AttachToWindow(window.GetHandle());

    input = std::make_unique<InputHandler>(window.GetHandle());
    shader = std::make_unique<Shader>("shaders/vertex-shader.glsl", "shaders/fragment-shader.glsl");
    renderer = std::make_unique<Renderer>(window.GetHandle());

    ui = std::make_unique<UIManager>();
    ui->Init(window.GetHandle());
    hud = std::make_unique<HUD>();
    mainMenu = std::make_unique<MainMenu>();
    pauseMenu = std::make_unique<PauseMenu>();

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
        player.ProcessInput(*input, camera, dt);
}

void Engine::SetState(State newState)
{
    state = newState;

    bool captured = (state == State::Playing);
    window.SetCursorCaptured(captured);
    camera.SetActive(captured);

    if (state == State::Playing)
        pauseMenu->Reset();
}

void Engine::StartWorld(const SaveData& data)
{
    currentSave = data;
    player.position = data.playerPosition;
    world = std::make_unique<World>(data.seed);
    world->Update(player.position);
}

void Engine::SaveWorld()
{
    currentSave.playerPosition = player.position;
    SaveManager::Save(currentSave.name, currentSave);
}

void Engine::Update(float dt)
{
    if (state != State::Playing)
        return;

    world->Update(player.position);
    player.Update(dt, *world);
    player.UpdateCamera(camera);
}

void Engine::Render()
{
    renderer->BeginFrame();

    if (state != State::Menu && world)
    {
        float aspectRatio = window.GetAspectRatio();
        glm::mat4 identity(1.0f);

        for (const auto& [key, data] : world->GetChunks())
        {
            if (data.mesh)
                renderer->Draw(*data.mesh, *shader, camera, identity, aspectRatio);
        }
    }

    bool interactive = (state != State::Playing);
    ui->BeginFrame(interactive);

    if (state == State::Menu)
    {
        auto action = mainMenu->Render(window, config, configPath);

        if (action == MainMenu::Action::Play)
        {
            StartWorld(mainMenu->GetWorldData());
            SetState(State::Playing);
        }
        else if (action == MainMenu::Action::Quit)
        {
            window.Close();
        }
    }
    else if (state == State::Paused)
    {
        hud->RenderOverlay(player.position, player.IsGrounded(), (int)world->GetChunks().size());

        auto action = pauseMenu->Render(window, config, configPath);

        if (action == PauseMenu::Action::Resume)
            SetState(State::Playing);
        else if (action == PauseMenu::Action::Quit)
        {
            SaveWorld();
            SetState(State::Menu);
        }
    }
    else
    {
        hud->RenderOverlay(player.position, player.IsGrounded(), (int)world->GetChunks().size());
        hud->RenderCrosshair();
    }

    ui->EndFrame();

    renderer->EndFrame();
}
