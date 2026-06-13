//
// Created by Andrea Pullia on 12/06/2026.
//

#pragma once
#include <string>

typedef unsigned int GLuint;
typedef float GLfloat;
typedef int GLint;

class Texture
{
public:
    virtual ~Texture() = default;

    Texture(const std::string* path, GLint TextureWrapMode, GLint MinifyingFilter, GLint MagnifyingFilter, bool ShouldGenerateMipMap = false);

    int GetWidth() const;
    int GetHeight() const;
    int GetNumberOfChannels() const;

    virtual void BindTexture() const;

protected:
    int width;
    int height;
    int channels;

    GLuint textureId;

    Texture();
};
