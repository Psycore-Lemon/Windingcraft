#include "core/Game.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/Config.h"

#include "graphics/Shader.h"
#include "graphics/Mesh.h"
#include "graphics/Renderer.h"
#include "graphics/HUD.h"

Game::Game()
    : player(glm::vec3(0.0f, 20.0f, 0.0f))
{
}

Game::~Game() = default;

bool Game::Init(const std::string& configPath)
{
    Config* config = ReadConfig(configPath);

    if (!window.Init(config->windowWidth, config->windowHeight, "Windingcraft"))
    {
        delete config;
        return false;
    }

    delete config;

    camera.AttachToWindow(window.GetHandle());

    input = std::make_unique<InputHandler>(window.GetHandle());
    shader = std::make_unique<Shader>("shaders/vertex-shader.glsl", "shaders/fragment-shader.glsl");
    renderer = std::make_unique<Renderer>(window.GetHandle());
    hud = std::make_unique<HUD>();
    hud->Init(window.GetHandle());

    world.Update(player.position);

    return true;
}

void Game::Run()
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

void Game::ProcessInput(float dt)
{
    input->Update();

    bool escDown = input->IsActionDown(Action::Pause);
    if (escDown && !escWasDown)
        SetPaused(!paused);
    escWasDown = escDown;

    if (!paused)
        player.ProcessInput(*input, camera, dt);
}

void Game::SetPaused(bool pause)
{
    paused = pause;
    window.SetCursorCaptured(!pause);
    camera.SetActive(!pause);
}

void Game::Update(float dt)
{
    if (paused)
        return;

    world.Update(player.position);
    player.Update(dt, world);
    player.UpdateCamera(camera);
}

void Game::Render()
{
    renderer->BeginFrame();

    float aspectRatio = window.GetAspectRatio();

    glm::mat4 identity(1.0f);

    for (const auto& [key, data] : world.GetChunks())
    {
        if (data.mesh)
            renderer->Draw(*data.mesh, *shader, camera, identity, aspectRatio);
    }

    hud->BeginFrame(paused);
    hud->RenderOverlay(player.position, player.IsGrounded(), (int)world.GetChunks().size());

    if (!paused)
        hud->RenderCrosshair();

    if (paused)
    {
        auto action = hud->RenderPauseMenu();

        if (action == HUD::PauseAction::Resume)
            SetPaused(false);
        else if (action == HUD::PauseAction::Quit)
            window.Close();
    }

    hud->EndFrame();

    renderer->EndFrame();
}
