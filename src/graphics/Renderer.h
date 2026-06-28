#pragma once

#include "graphics/Mesh.h"
#include "graphics/Shader.h"
#include "scene/Camera.h"
#include "scene/Transform.h"

#include <glm/glm.hpp>
#include <glfw/glfw3.h>

class Renderer
{
public:
    Renderer(GLFWwindow* window);

    void BeginFrame(float r = 0.1f, float g = 0.15f, float b = 0.2f, float a = 1.0f);
    void Draw(const Mesh& mesh, const Shader& shader, const Camera& camera, const Transform& transform, float aspectRatio);
    void EndFrame();

private:
    GLFWwindow* window;
};