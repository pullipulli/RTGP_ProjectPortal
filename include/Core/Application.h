//
// Created by Andrea Pullia on 17/02/2026.
//

#pragma once
#include "glm/vec3.hpp"
#include "RenderTexture.h"

class Camera;
class Input;
class Physics;
class Model;
class Shader;

typedef float GLfloat;

class Application
{
public:
    const float FIXED_DELTA_TIME = 1/60.f;
    Application(float screenWidth, float screenHeight);
    void StartApplication();
    void DrawScene(RenderPass currentRenderPass);
private:
    int screenWidth = 1920;
    int screenHeight = 1080;

    float deltaTime = 0;
    float lastFrameTime = 0;
    float currentFrameTime = 0;

    Camera *camera;
    Physics *bulletSimulation;

    void ApplyCameraMovements();
    Input *input;

    // Blinn-Phong equation parameters
    glm::vec3 ambientColor;

    glm::vec3 lightPos0 = glm::vec3(-5.0f, 5.0f, 1.0f);

    RenderTexture* renderTexture;

    Model* cubeModel;
    Model* portalModel;

    Shader* globalShader;
};
