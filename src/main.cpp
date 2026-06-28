#include <iostream>
#include <glad/gl.h>
#include <glfw/glfw3.h>

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

const std::string CONFIG_FILEPATH = "config.json";

Camera camera({0.0f, 0.0f, 3.0f});
float lastX = 0.0f, lastY = 0.0f;
bool  firstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) { lastX = (float)xpos; lastY = (float)ypos; firstMouse = false; }
    float dx = (float)xpos - lastX;
    float dy = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;
    camera.ProcessMouseMovement(dx, dy);
}

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

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1);

    std::cout << "Hello CRPG Prototype!\n";

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

            glm::vec2 mouseDelta = input.GetMouseDelta();
            camera.ProcessMouseMovement(mouseDelta.x, -mouseDelta.y);

            if (input.IsKeyDown(GLFW_KEY_W))
                camera.MoveForward(dt);

            if (input.IsKeyDown(GLFW_KEY_S))
                camera.MoveBackward(dt);

            if (input.IsKeyDown(GLFW_KEY_A))
                camera.MoveLeft(dt);

            if (input.IsKeyDown(GLFW_KEY_D))
                camera.MoveRight(dt);

            if (input.IsActionDown(Action::Pause))
            {
                glfwSetWindowShouldClose(window, true);
            }

            renderer.BeginFrame();

            for (int x = -1; x <= 1; ++x) {
                for (int z = -1; z <= 1; ++z) {
                    glm::mat4 model = glm::translate(glm::mat4(1.0f),
                        glm::vec3(x * 1.0f, 0.0f, z * 1.0f));
                    renderer.Draw(cube, shader, camera, model, aspectRatio);
                }
            }

            renderer.EndFrame();
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    delete config;

    return 0;
}
