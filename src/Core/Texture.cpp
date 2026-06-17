//
// Created by Andrea Pullia on 12/06/2026.
//


#include "Core/Texture.h"
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#include <iostream>

Texture::Texture(const std::string* path, GLint TextureWrapMode, GLint MinifyingFilter, GLint MagnifyingFilter, bool ShouldGenerateMipMap)
{
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureWrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureWrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinifyingFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagnifyingFilter);

    if (path == nullptr) return;

    unsigned char* textureData = stbi_load(path->c_str(), &width, &height, &channels, 0);

    if (textureData)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
        if (ShouldGenerateMipMap) glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Unable to create texture.";
    }

    stbi_image_free(textureData);

    textureResourceId = *path;
}

int Texture::GetWidth() const
{
    return width;
}

int Texture::GetHeight() const {
    return height;
}

int Texture::GetNumberOfChannels() const
{
    return channels;
}

void Texture::BindTexture() const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
}

const std::string & Texture::GetTextureResourceId() const
{
    return textureResourceId;
}

Texture::Texture() : Texture(nullptr, GL_REPEAT, GL_LINEAR, GL_LINEAR, false)
{
}
