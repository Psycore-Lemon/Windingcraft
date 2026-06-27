#include <iostream>
#include <GLAD/gl.h>
#include <GLFW/glfw3.h>

#include <string>

#include <nlohmann/json.hpp>

#include "core/Config.h"

const std::string CONFIG_FILEPATH = "config.json";


int main()
{
    
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE);

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
    {
        return -1;
    }

    std::cout << "Hello CRPG Prototype!\n";

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(
            0.1f,
            0.1f,
            0.2f,
            1.0f
        );

        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    delete config;
    
    return 0;
}