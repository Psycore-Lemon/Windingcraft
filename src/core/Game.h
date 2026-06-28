#pragma once

#include <memory>
#include <string>

#include "core/Window.h"
#include "core/Time.h"
#include "core/InputHandler.h"

#include "scene/Camera.h"
#include "scene/Player.h"
#include "scene/World.h"

class Shader;
class Mesh;
class Renderer;

class Game
{
public:
    Game();
    ~Game();

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    bool Init(const std::string& configPath);
    void Run();

private:
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();

    Window window;

    Camera camera;
    Player player;
    World world;

    Time time;
    std::unique_ptr<InputHandler> input;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Mesh> cubeMesh;
    std::unique_ptr<Renderer> renderer;
};
