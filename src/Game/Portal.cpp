//
// Created by Andrea Pullia on 14/11/2026.
//

#include "Game/Portal.h"

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
}
