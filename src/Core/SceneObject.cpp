//
// Created by Andrea Pullia on 17/06/2026.
//

#include "Core/SceneObject.h"

#include "Core/Material.h"
#include "Core/ResourceManager.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.inl"
#include "utils/model.h"

std::vector<SceneObject*> SceneObject::sceneObjects{};

glm::mat4 SceneObject::CalculateModelMatrix() const
{
    glm::mat4 modelMatrix{1};

    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    modelMatrix = glm::scale(modelMatrix, scale);

    return modelMatrix;
}

void SceneObject::Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, RenderPass renderPass)
{
    material->GetShader().SetUniformParameter("projectionMatrix", &projectionMatrix);
    material->GetShader().SetUniformParameter("projectionMatrix", &projectionMatrix);
    material->GetShader().SetUniformParameter("viewMatrix", &viewMatrix);
    material->GetShader().SetUniformParameter("pointLightPosition", &lightPos0);
    material->GetShader().SetUniformParameter("ambientColor", &ambientColor);

    glm::mat4 modelMatrix = CalculateModelMatrix();
    // if we cast a mat4 to a mat3, we are automatically considering the upper left 3x3 submatrix
    glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(viewMatrix*modelMatrix));

    material->GetShader().SetUniformParameter("modelMatrix", &modelMatrix);
    material->GetShader().SetUniformParameter("normalMatrix", &normalMatrix);

    material->Use();
    model->Draw();
}

const std::vector<SceneObject*> SceneObject::GetAllActiveSceneObjects()
{
    // TODO handle object despawn and destroy

    return sceneObjects;
}

SceneObject::SceneObject(const glm::vec3 &initialPosition, const glm::vec3 &initialRotation, const glm::vec3 &initialScale, Material* material, const std::string& modelResourceId)
{
    position = initialPosition;
    rotation = initialRotation;
    scale = initialScale;
    this->material = material;

    if (!modelResourceId.empty())
        model = ResourceManager::GetInstance()->GetModel(modelResourceId);

    sceneObjects.push_back(this);
}
