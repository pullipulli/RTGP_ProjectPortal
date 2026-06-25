//
// Created by Andrea Pullia on 17/06/2026.
//

#pragma once
#include "glm/glm.hpp"
#include <string>
#include "RenderTexture.h"

class SceneObject
{
public:
    virtual ~SceneObject() = default;

    SceneObject(const glm::vec3& initialPosition, const glm::vec3& initialRotation, const glm::vec3& initialScale, class Material* material = nullptr, const std::string& modelResourceId = "");

    /// Called after the construction of all SceneObjects
    virtual void Start() {}

    /// Called for each frame
    virtual void Update(float deltaTime) {}

    /// Calculate model and normal matrices, then render everything on the screen for this frame
    virtual void Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, RenderPass renderPass);

    static const std::vector<SceneObject*> GetAllActiveSceneObjects();
protected:
    glm::vec3 position{0};
    glm::vec3 rotation{0};
    glm::vec3 scale{1};
    class Material* material = nullptr;
    class Model* model = nullptr;

private:
    glm::mat4 CalculateModelMatrix() const;

    //TODO: add rigidbody!

    static std::vector<SceneObject*> sceneObjects;
};
