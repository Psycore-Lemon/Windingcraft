#pragma once
#include <glm/glm.hpp>

// A unit cube (1x1x1) centered on the origin.
// Owns its own GPU buffers (VAO + VBO) and frees them on destruction.
class Cube {
public:
    Cube();
    ~Cube();

    // Non-copyable: copying would duplicate the GPU handle IDs, and when
    // one copy is destroyed it would delete buffers the other still uses.
    Cube(const Cube&) = delete;
    Cube& operator=(const Cube&) = delete;

    // Move is fine: transfers ownership of the handles, leaves the
    // source empty so its destructor is a no-op.
    Cube(Cube&& other) noexcept;
    Cube& operator=(Cube&& other) noexcept;

    void draw() const;

private:
    void release();   // helper: delete buffers if we own any

    unsigned int m_vao = 0;   // VAO: the "recipe" — how to read the VBO
    unsigned int m_vbo = 0;   // VBO: the raw vertex data on the GPU
};