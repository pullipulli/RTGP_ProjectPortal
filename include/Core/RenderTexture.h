//
// Created by Andrea Pullia on 24/02/2026.
//

#pragma once
#include "Texture.h"

typedef unsigned int GLuint;
typedef float GLfloat;
typedef int GLint;

class RenderTexture : public Texture
{
public:
    RenderTexture(GLint width, GLint height, const std::string& renderTextureResourceId);
    void BindFrameBuffer() const;
private:
    GLuint rtFrameBufferId;
    GLuint depthBufferId;
};

enum class RenderPass
{
    Screen,
    RenderTexture
};