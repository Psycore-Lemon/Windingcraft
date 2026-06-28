#include "graphics/PrimitiveMeshes.h"

#include <vector>

Mesh PrimitiveMeshes::CreateTriangle()
{
    std::vector<float> vertices =
    {
        // positions          // UVs
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
         0.0f,  0.5f, 0.0f,   0.5f, 1.0f
    };

    std::vector<unsigned int> indices =
    {
        0, 1, 2
    };

    return Mesh(vertices, indices);
}

Mesh PrimitiveMeshes::CreateCube()
{
    std::vector<float> vertices =
    {
        // positions             // UVs

        // front face
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,

        // back face
         0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,    0.0f, 1.0f,

        // left face
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,

        // right face
         0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    0.0f, 1.0f,

        // top face
        -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,

        // bottom face
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 1.0f,
    };

    std::vector<unsigned int> indices =
    {
         0,  1,  2,  2,  3,  0,   // front
         4,  5,  6,  6,  7,  4,   // back
         8,  9, 10, 10, 11,  8,   // left
        12, 13, 14, 14, 15, 12,   // right
        16, 17, 18, 18, 19, 16,   // top
        20, 21, 22, 22, 23, 20    // bottom
    };

    return Mesh(vertices, indices);
}