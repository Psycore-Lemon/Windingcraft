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
    // 24 vertices (4 per face), each: position (vec3) + edge coord (vec2)
    // Edge coords (0,0)→(1,0)→(1,1)→(0,1) per quad for border detection
    std::vector<float> vertices =
    {
        // Back face (z = -0.5)
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,

        // Front face (z = 0.5)
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,

        // Left face (x = -0.5)
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        // Right face (x = 0.5)
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f,

        // Bottom face (y = -0.5)
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        // Top face (y = 0.5)
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    };

    std::vector<unsigned int> indices =
    {
        0,  1,  2,  2,  3,  0,   // back
        4,  5,  6,  6,  7,  4,   // front
        8,  9,  10, 10, 11, 8,   // left
        12, 13, 14, 14, 15, 12,  // right
        16, 17, 18, 18, 19, 16,  // bottom
        20, 21, 22, 22, 23, 20,  // top
    };

    return Mesh(vertices, indices, 5);
}
