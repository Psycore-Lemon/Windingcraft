#pragma once

#include <glad/gl.h>
#include <string>
#include <vector>

class TextureArray
{
public:
    TextureArray(const std::vector<std::string>& paths, int tileSize);
    ~TextureArray();

    TextureArray(const TextureArray&) = delete;
    TextureArray& operator=(const TextureArray&) = delete;

    void Bind(unsigned int slot = 0) const;

private:
    unsigned int ID = 0;
};
