#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <nlohmann/json.hpp>

// Core
#include "core/Config.h"
#include "core/Time.h"
#include "core/InputHandler.h"

#include "graphics/Shader.h"
#include "graphics/Mesh.h"
#include "graphics/PrimitiveMeshes.h"
#include "graphics/Renderer.h"

#include "scene/Camera.h"
#include "scene/Player.h"
#include "scene/World.h"

const std::string CONFIG_FILEPATH = "config.json";

int main()
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Config* config = ReadConfig(CONFIG_FILEPATH);

    GLFWwindow* window = glfwCreateWindow(
        config->windowWidth,
        config->windowHeight,
        "Windingcraft",
        nullptr,
        nullptr
    );

    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGL(glfwGetProcAddress))
        return -1;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1);

    std::cout << "Hello CRPG Prototype!\n";

    World world;
    for (int x = -1; x <= 1; ++x)
        for (int z = -1; z <= 1; ++z)
            world.AddBlock(glm::vec3(x, 0, z));

    Camera camera;
    camera.AttachToWindow(window);

    Player player(glm::vec3(0.0f, 2.0f, 0.0f));

    {
        Time time;
        InputHandler input(window);
        Shader shader("shaders/vertex-shader.glsl", "shaders/fragment-shader.glsl");

        Mesh cube = PrimitiveMeshes::CreateCube();

        Renderer renderer(window);

        float aspectRatio = 1280.0f / 720.0f;

        while (!glfwWindowShouldClose(window))
        {
            time.Update();
            float dt = static_cast<float>(time.DeltaTime());

            glfwPollEvents();

            input.Update();

            glm::vec3 cameraFront = camera.GetFront();

            if (input.IsKeyDown(GLFW_KEY_W))
                player.MoveForward(dt, cameraFront);

            if (input.IsKeyDown(GLFW_KEY_S))
                player.MoveBackward(dt, cameraFront);

            if (input.IsKeyDown(GLFW_KEY_A))
                player.MoveLeft(dt, cameraFront);

            if (input.IsKeyDown(GLFW_KEY_D))
                player.MoveRight(dt, cameraFront);

            if (input.IsActionDown(Action::Jump))
                player.Jump();

            if (input.IsActionDown(Action::Pause))
            {
                glfwSetWindowShouldClose(window, true);
            }

            player.Update(dt, world);
            player.UpdateCamera(camera);

            renderer.BeginFrame();

            for (const auto& block : world.GetBlocks())
            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), block);
                renderer.Draw(cube, shader, camera, model, aspectRatio);
            }

            renderer.EndFrame();
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    delete config;

    return 0;
}
