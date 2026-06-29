#include "graphics/Mesh.h"

Mesh::Mesh(const std::vector<float>& vertices,
           const std::vector<unsigned int>& indices,
           unsigned int floatsPerVertex)
{
    indexCount = static_cast<unsigned int>(indices.size());

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );

    unsigned int stride = floatsPerVertex * sizeof(float);

    // location 0 = vec3 position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // location 1 = vec2 texcoord / edge coordinate (if present)
    if (floatsPerVertex >= 5)
    {
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    // location 2 = float texture layer (stride 6) or vec3 color (stride 8+)
    if (floatsPerVertex >= 6 && floatsPerVertex < 8)
    {
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }
    else if (floatsPerVertex >= 8)
    {
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
}

Mesh::Mesh(Mesh&& other) noexcept
    : VAO(other.VAO), VBO(other.VBO), EBO(other.EBO), indexCount(other.indexCount)
{
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
    other.indexCount = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if (this != &other)
    {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);

        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;
        indexCount = other.indexCount;

        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
        other.indexCount = 0;
    }
    return *this;
}

void Mesh::Draw() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
