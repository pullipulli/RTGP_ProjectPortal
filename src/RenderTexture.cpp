//
// Created by Andrea Pullia on 24/02/2026.
// Created following this guide https://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
//

#include "../include/RenderTexture.h"

#include <glad/glad.h>
#include "utils/shader.h"
#include <../glm/glm.hpp>

RenderTexture::RenderTexture()
{
    // setup of the texture to be rendered dynamically
    glGenBuffers(1, &rtFrameBufferId);
    glBindBuffer(GL_FRAMEBUFFER, rtFrameBufferId);

    glGenTextures(1, &renderedTextureId);
    glBindTexture(GL_TEXTURE_2D, renderedTextureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 1024, 768, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glGenRenderbuffers(1, &depthBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferId);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTextureId, 0);

    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);

    // setup of the quad where the texture will be rendered

    glGenVertexArrays(1, &quadVertexArrayId);
    glBindVertexArray(quadVertexArrayId);

    glGenBuffers(1, &quadVertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, quadVertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertexBufferData), quadVertexBufferData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));

    appliedShader = new Shader("shaders/vertex/renderTextureShader.vert", "shaders/fragment/renderTextureShader.frag");

    appliedShader->SetUniformParameter("renderTextureSampler", static_cast<int>(renderedTextureId));
}

void RenderTexture::SetupMVPMatrices(glm::mat4 *projectionMatrix, glm::mat4 *viewMatrix,
    glm::mat4 *modelMatrix, glm::mat3 *normalMatrix)
{
    appliedShader->SetUniformParameter("projectionMatrix", projectionMatrix);
    appliedShader->SetUniformParameter("viewMatrix", viewMatrix);
    appliedShader->SetUniformParameter("modelMatrix", modelMatrix);
    appliedShader->SetUniformParameter("normalMatrix", normalMatrix);
}

void RenderTexture::SetupBlinnPhongShader(glm::vec3 *pointLightPosition, glm::vec3 *ambientColor,
    glm::vec3* diffuseColor, glm::vec3 *specularColor, float Ka, float Kd, float Ks, float shininess)
{
    appliedShader->SetUniformParameter("pointLightPosition", pointLightPosition);
    appliedShader->SetUniformParameter("ambientColor", ambientColor);
    appliedShader->SetUniformParameter("diffuseColor", diffuseColor);
    appliedShader->SetUniformParameter("specularColor", specularColor);
    appliedShader->SetUniformParameter("Ka", Ka);
    appliedShader->SetUniformParameter("Kd", Kd);
    appliedShader->SetUniformParameter("Ks", Ks);
    appliedShader->SetUniformParameter("shininess", shininess);
}

void RenderTexture::Draw()
{
    appliedShader->Use();
    glBindVertexArray(this->quadVertexArrayId);
    glDrawArrays(GL_TRIANGLES, 0, 4);
}


