#include "graphics/PrimitiveMeshes.h"

#include <vector>

Mesh PrimitiveMeshes::CreateTriangle()
{
    std::vector<float> vertices =
    {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
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
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f
    };

    std::vector<unsigned int> indices =
    {
        0, 1, 2, 2, 3, 0, // back
        4, 5, 6, 6, 7, 4, // front
        4, 7, 3, 3, 0, 4, // left
        1, 5, 6, 6, 2, 1, // right
        4, 5, 1, 1, 0, 4, // bottom
        3, 2, 6, 6, 7, 3  // top
    };

    return Mesh(vertices, indices);
}