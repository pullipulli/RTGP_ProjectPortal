//
// Created by Andrea Pullia on 17/02/2026.
//

#pragma once

class Camera;

class Application
{
public:
    Application(float screenWidth, float screenHeight);
    void StartApplication();
private:
    int screenWidth = 1920;
    int screenHeight = 1080;

    float deltaTime = 0;
    float lastFrameTime = 0;
    float currentFrameTime = 0;

    Camera *camera;
};
