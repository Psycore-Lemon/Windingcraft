#include "Cube.h"
#include <glad/gl.h>
#include <utility>   // std::exchange

namespace {
    // 36 vertices (6 faces x 2 triangles x 3 verts).
    // Each vertex is 5 floats: position (x, y, z) + texcoord (u, v).
    // The UVs are kept so the cube is texture-ready; the current shader
    // just doesn't sample from them yet (it colors by position instead).
    const float cubeVertices[] = {
        // back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   0.5f, -0.5f, -0.5f,  1.0f, 0.0f,   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        // front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        // left face
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  -0.5f,  0.5f, -0.5f,  1.0f, 0.0f,  -0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        // right face
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f,   0.5f,  0.5f, -0.5f,  1.0f, 0.0f,   0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,   0.5f, -0.5f,  0.5f,  0.0f, 1.0f,   0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        // bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   0.5f, -0.5f, -0.5f,  1.0f, 0.0f,   0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        // top face
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,   0.5f,  0.5f, -0.5f,  1.0f, 0.0f,   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
    };
}

Cube::Cube() {
    // Create the two objects and get their integer handles.
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    // Bind the VAO first — every vertex-layout call below gets recorded into it.
    glBindVertexArray(m_vao);

    // Bind the VBO and upload the vertex data into GPU memory.
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // Stride is now 5 floats per vertex (3 position + 2 texcoord).
    // attribute 0 = position -> layout(location = 0) in vec3 aPos;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // attribute 1 = texcoord -> layout(location = 1) in vec2 aTexCoord;
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind for safety so later code doesn't accidentally modify this VAO.
    glBindVertexArray(0);
}

void Cube::release() {
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    m_vao = 0;
    m_vbo = 0;
}

Cube::~Cube() {
    release();
}

Cube::Cube(Cube&& other) noexcept
    : m_vao(std::exchange(other.m_vao, 0)),
      m_vbo(std::exchange(other.m_vbo, 0)) {
    // other is left with zeroed handles, so its destructor does nothing.
}

Cube& Cube::operator=(Cube&& other) noexcept {
    if (this != &other) {
        release();                              // free what we currently own
        m_vao = std::exchange(other.m_vao, 0);  // steal other's handles
        m_vbo = std::exchange(other.m_vbo, 0);
    }
    return *this;
}

void Cube::draw() const {
    // Binding the VAO restores the VBO link + layout in one call.
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}