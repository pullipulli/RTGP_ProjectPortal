//
// Created by Andrea Pullia on 14/11/2026.
//

#include "Game/Portal.h"

Portal::Portal(const glm::vec3 &initialPosition, const glm::vec3 &initialRotation, const glm::vec3 &initialScale,
    Material *material, const std::string &modelResourceId, bool hasRigidbody, Camera *thisPortalCamera)
    : SceneObject(initialPosition, initialRotation, initialScale, material, modelResourceId, hasRigidbody),
          thisPortalCamera(thisPortalCamera),
          renderTexture(dynamic_cast<RenderTexture*>(material->GetTexture()))
{
}

void Portal::Start()
{
    SceneObject::Start();
}

void Portal::Update(float deltaTime)
{
    SceneObject::Update(deltaTime);
}

void Portal::Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, RenderPass renderPass)
{
    SceneObject::Render(viewMatrix, projectionMatrix, renderPass);
}

void Portal::LinkOtherPortal(Portal *portal)
{
    linkedPortal = portal;
    thisPortalCamera->UpdatePosition(linkedPortal->position);
}
