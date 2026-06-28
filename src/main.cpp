#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <nlohmann/json.hpp>

// Core
#include "core/Config.h"
#include "core/Camera.h"
#include "core/Shader.h"

// Models
#include "models/Cube.h"

const std::string CONFIG_FILEPATH = "config.json";

// File-scope so the mouse callback can reach it.
Camera camera({0.0f, 0.0f, 3.0f});
float lastX = 0.0f, lastY = 0.0f;
bool  firstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) { lastX = (float)xpos; lastY = (float)ypos; firstMouse = false; }
    float dx = (float)xpos - lastX;
    float dy = lastY - (float)ypos;   // reversed: screen y grows downward
    lastX = (float)xpos;
    lastY = (float)ypos;
    camera.processMouse(dx, dy);
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
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGL(glfwGetProcAddress))
        return -1;

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // capture mouse

    glEnable(GL_DEPTH_TEST);   // needed so cube faces sort correctly

    std::cout << "Hello CRPG Prototype!\n";

    // GL objects must be created AFTER the context exists.
    Shader shader("shaders/shader.vert", "shaders/shader.frag");
    Cube   cube;

    float lastFrameTime = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float now = glfwGetTime();
        float dt  = now - lastFrameTime;
        lastFrameTime = now;

        // input
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.processKeyboard(GLFW_KEY_W, dt);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.processKeyboard(GLFW_KEY_S, dt);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.processKeyboard(GLFW_KEY_A, dt);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.processKeyboard(GLFW_KEY_D, dt);

        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);

        shader.use();
        shader.setMat4("projection", camera.projection((float)w / h));
        shader.setMat4("view", camera.view());

        for (int x = -1; x <= 1; ++x) {
            for (int z = -1; z <= 1; ++z) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f),
                    glm::vec3(x * 2.0f, 0.0f, z * 2.0f));
                shader.setMat4("model", model);
                cube.draw();
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    delete config;

    return 0;
}