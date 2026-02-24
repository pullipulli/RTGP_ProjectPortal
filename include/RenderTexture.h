//
// Created by Andrea Pullia on 24/02/2026.
//

#pragma once
#include "fwd.hpp"

typedef unsigned int GLuint;
typedef float GLfloat;

class RenderTexture
{
public:
    RenderTexture();
    void SetupMVPMatrices(glm::mat4* projectionMatrix, glm::mat4* viewMatrix, glm::mat4* modelMatrix, glm::mat3* normalMatrix);
    void SetupBlinnPhongShader(glm::vec3* pointLightPosition, glm::vec3* ambientColor, glm::vec3* diffuseColor,  glm::vec3* specularColor, float Ka, float Kd, float Ks, float shininess);
    void Draw();

private:
    GLuint rtFrameBufferId;
    GLuint renderedTextureId;
    GLuint depthBufferId;

    GLuint quadVertexArrayId;
    GLuint quadVertexBufferId;
    const GLfloat quadVertexBufferData[18] =
    {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };

    const GLfloat UVCoordinates[8] =
    {
        0.f, 0.f,
        0.f, 1.f,
        1.f, 1.f,
        1.f, 0.f
    };

    class Shader* appliedShader;
};
