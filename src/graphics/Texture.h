#pragma once

#include <glad/gl.h>
#include <string>

class Texture
{
public:
    Texture(const std::string& path);
    ~Texture();

    void Bind(unsigned int slot = 0) const;

private:
    unsigned int ID = 0;
};