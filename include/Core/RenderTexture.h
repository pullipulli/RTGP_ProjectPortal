//
// Created by Andrea Pullia on 24/02/2026.
//

#pragma once

typedef unsigned int GLuint;
typedef float GLfloat;
typedef int GLint;

class RenderTexture
{
public:
    RenderTexture(GLint width, GLint height);
    void BindFrameBuffer();
    void BindFrameBufferTexture();

private:
    GLint width;
    GLint height;

    GLuint rtFrameBufferId;
    GLuint renderedTextureId;
    GLuint depthBufferId;
};

enum class RenderPass
{
    Screen,
    RenderTexture
};