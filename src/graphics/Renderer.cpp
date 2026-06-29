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
    float aspectRatio) const
{
    shader.Use();

    shader.SetMat4("model", model);
    shader.SetMat4("view", camera.GetViewMatrix());
    shader.SetMat4("projection", camera.GetProjectionMatrix(aspectRatio));

    mesh.Draw();
}

void Renderer::DrawBlockHighlight(
    const Shader& shader,
    const Camera& camera,
    const glm::ivec3& blockPos,
    float aspectRatio) const
{
    float e = 0.002f;
    float x0 = (float)blockPos.x - e;
    float y0 = (float)blockPos.y - e;
    float z0 = (float)blockPos.z - e;
    float x1 = x0 + 1.0f + 2*e;
    float y1 = y0 + 1.0f + 2*e;
    float z1 = z0 + 1.0f + 2*e;

    float lines[] = {
        // bottom face
        x0,y0,z0, x1,y0,z0,  x1,y0,z0, x1,y0,z1,
        x1,y0,z1, x0,y0,z1,  x0,y0,z1, x0,y0,z0,
        // top face
        x0,y1,z0, x1,y1,z0,  x1,y1,z0, x1,y1,z1,
        x1,y1,z1, x0,y1,z1,  x0,y1,z1, x0,y1,z0,
        // verticals
        x0,y0,z0, x0,y1,z0,  x1,y0,z0, x1,y1,z0,
        x1,y0,z1, x1,y1,z1,  x0,y0,z1, x0,y1,z1,
    };

    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_STREAM_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    shader.Use();
    shader.SetMat4("model", glm::mat4(1.0f));
    shader.SetMat4("view", camera.GetViewMatrix());
    shader.SetMat4("projection", camera.GetProjectionMatrix(aspectRatio));

    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, 24);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void Renderer::EndFrame()
{
    glfwSwapBuffers(window);
}