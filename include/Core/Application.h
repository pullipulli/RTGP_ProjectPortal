//
// Created by Andrea Pullia on 17/02/2026.
//

#pragma once
#include <memory>

#include "glm/vec3.hpp"
#include "Core/Graphics/Texture/RenderTexture.h"

class Camera;
class Input;
class Physics;
class Model;
class Shader;
class Material;
class Portal;

typedef float GLfloat;

class Application
{
public:
    const float FIXED_DELTA_TIME = 1/60.f;
    Application(float screenWidth, float screenHeight);
    ~Application();
    void StartApplication();
    void DrawScene(const Camera* renderingCamera, RenderPass currentRenderPass) const;
private:
    float screenWidth = 1920;
    float screenHeight = 1080;

    float deltaTime = 0;
    float lastFrameTime = 0;
    float currentFrameTime = 0;

    std::unique_ptr<Camera> camera;
    std::unique_ptr<Portal> bluePortal = nullptr;
    std::unique_ptr<Portal> redPortal = nullptr;

    void ApplyCameraMovements();
    std::unique_ptr<Input> input;
};
