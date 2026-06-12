//
// Created by Andrea Pullia on 24/02/2026.
// Created following this guide https://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
//

#include "Core/RenderTexture.h"

#include <glad/glad.h>
#include "utils/shader.h"
#include <glm/glm.hpp>
#include <cassert>

RenderTexture::RenderTexture(GLint width, GLint height)
{
    this->width = width;
    this->height = height;

    // setup of frame buffer
    glGenFramebuffers(1, &rtFrameBufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, rtFrameBufferId);

    // setup of the dynamic texture
    glGenTextures(1, &renderedTextureId);
    glBindTexture(GL_TEXTURE_2D, renderedTextureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTextureId,0 );

    // setup for the depth buffer
    glGenRenderbuffers(1, &depthBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferId);

    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE && "Render Texture Framebuffer Initialization failed");
}

void RenderTexture::BindFrameBuffer()
{
    // draws the current  camera view to buffer
    glBindFramebuffer(GL_FRAMEBUFFER, rtFrameBufferId);
    glClearColor(1, 0, 0, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);
}

void RenderTexture::BindFrameBufferTexture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderedTextureId);
}


