#include "graphics/Renderer.h"

#include <glad/gl.h>

Renderer::Renderer(GLFWwindow* window)
    : window(window)
{
    glEnable(GL_DEPTH_TEST);
}

void Renderer::BeginFrame(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Draw(
    const Mesh& mesh,
    const Shader& shader,
    const Camera& camera,
    const glm::mat4& model,
    float aspectRatio)
{
    shader.Use();

    shader.SetMat4("model", model);
    shader.SetMat4("view", camera.GetViewMatrix());
    shader.SetMat4("projection", camera.GetProjectionMatrix(aspectRatio));

    mesh.Draw();
}

void Renderer::EndFrame()
{
    glfwSwapBuffers(window);
}