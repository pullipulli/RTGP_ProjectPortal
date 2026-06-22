//
// Created by Andrea Pullia on 17/02/2026.
//

#pragma once
#include <memory>

#include "glm/vec3.hpp"
#include "RenderTexture.h"

class Camera;
class Input;
class Physics;
class Model;
class Shader;
class Material;

typedef float GLfloat;

class Application
{
public:
    const float FIXED_DELTA_TIME = 1/60.f;
    Application(float screenWidth, float screenHeight);
    ~Application();
    void StartApplication();
    void DrawScene(RenderPass currentRenderPass);
private:
    int screenWidth = 1920;
    int screenHeight = 1080;

    float deltaTime = 0;
    float lastFrameTime = 0;
    float currentFrameTime = 0;

    std::unique_ptr<Camera> camera;
    std::unique_ptr<Physics> bulletSimulation;

    void ApplyCameraMovements();
    std::unique_ptr<Input> input;

    // Blinn-Phong equation parameters. To be replaced with arrays of lights
    glm::vec3 ambientColor;
    glm::vec3 lightPos0 = glm::vec3(-5.0f, 5.0f, 1.0f);
};
