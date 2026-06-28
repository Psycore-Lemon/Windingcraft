#pragma once

#include <glad/gl.h>
#include <vector>

class Mesh
{
public:
    Mesh(const std::vector<float>& vertices,
         const std::vector<unsigned int>& indices,
         unsigned int floatsPerVertex = 3);

    ~Mesh();

    void Draw() const;

private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    unsigned int indexCount = 0;
};