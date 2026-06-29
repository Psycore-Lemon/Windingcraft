#include "graphics/TextureArray.h"

#include <stb_image.h>
#include <cstdio>

TextureArray::TextureArray(const std::vector<std::string>& paths, int tileSize)
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D_ARRAY, ID);

    int layerCount = static_cast<int>(paths.size());
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8,
                 tileSize, tileSize, layerCount,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    stbi_set_flip_vertically_on_load(true);

    for (int i = 0; i < layerCount; ++i)
    {
        int w, h, channels;
        unsigned char* data = stbi_load(paths[i].c_str(), &w, &h, &channels, 4);
        if (!data)
        {
            std::printf("Failed to load texture: %s\n", paths[i].c_str());
            continue;
        }

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
                        0, 0, i,
                        tileSize, tileSize, 1,
                        GL_RGBA, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}

TextureArray::~TextureArray()
{
    if (ID) glDeleteTextures(1, &ID);
}

void TextureArray::Bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D_ARRAY, ID);
}
