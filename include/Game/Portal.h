//
// Created by Andrea Pullia on 14/11/2026.
//

#pragma once
#include "Core/Camera.h"
#include "Core/SceneObject.h"
#include "Core/Graphics/Material.h"


class Camera;

class Portal : SceneObject
{
public:
    Portal(const glm::vec3 &initialPosition, const glm::vec3 &initialRotation, const glm::vec3 &initialScale,
        Material *material, const std::string &modelResourceId, bool hasRigidbody, Camera *thisPortalCamera);

    ~Portal() override = default;

    void Start() override;

    void Update(float deltaTime) override;

    void Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, RenderPass renderPass) override;

    void LinkOtherPortal(Portal* portal);

    [[nodiscard]] RenderTexture* GetRenderTexture() const { return renderTexture; }
    [[nodiscard]] Camera* GetThisPortalCamera() const { return thisPortalCamera; }

private:
    Portal* linkedPortal = nullptr;
    Camera* thisPortalCamera;
    RenderTexture* renderTexture;
    bool isPortalPlaced = false;
};
