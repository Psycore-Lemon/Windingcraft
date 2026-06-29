#pragma once

#include "graphics/Mesh.h"
#include "graphics/Shader.h"
#include "scene/Camera.h"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Renderer
{
public:
    Renderer(GLFWwindow* window);

    void BeginFrame(float r = 0.1f, float g = 0.15f, float b = 0.2f, float a = 1.0f);
    void Draw(const Mesh& mesh, const Shader& shader, const Camera& camera, const glm::mat4& model, float aspectRatio) const;
    void DrawBlockHighlight(const Shader& shader, const Camera& camera, const glm::ivec3& blockPos, float aspectRatio) const;
    void EndFrame();

private:
    GLFWwindow* window;
};
