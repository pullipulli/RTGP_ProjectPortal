//
// Created by Andrea Pullia on 17/06/2026.
//

#include "Core/Material.h"

#include "Core/ResourceManager.h"
#include "utils/shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Core/PointLight.h"
#include "Core/Texture.h"

Material Material::Create(const std::string& shaderId)
{
    return Material{shaderId};
}

Material::Material(const std::string& shaderId)
{
    shader = ResourceManager::GetInstance()->GetShader(shaderId);
}

void Material::Use() const
{
    shader->SetUniformParameter("material.ambient", &ambientColor);
    shader->SetUniformParameter("material.diffuse", &diffuseColor);
    shader->SetUniformParameter("material.specular", &specularColor);
    shader->SetUniformParameter("material.shininess", shininess);
    shader->SetUniformParameter("material.hasTexture", texture != nullptr);

    // TODO handle multiple lights at once!
    shader->SetUniformParameter("light.ambient", &PointLight::lights[0]->ambientColor);
    shader->SetUniformParameter("light.diffuse", &PointLight::lights[0]->diffuseColor);
    shader->SetUniformParameter("light.specular", &PointLight::lights[0]->specularColor);
    shader->SetUniformParameter("light.Ka", PointLight::lights[0]->Ka);
    shader->SetUniformParameter("light.Kd", PointLight::lights[0]->Kd);
    shader->SetUniformParameter("light.Ks", PointLight::lights[0]->Ks);
    shader->SetUniformParameter("light.position", &PointLight::lights[0]->position);

    if (texture)
        texture->BindTexture();
    shader->Use();
}

Material & Material::AddAmbient(const glm::vec3 &ambient)
{
    ambientColor = glm::vec3(ambient);

    return *this;
}

Material& Material::AddDiffuse(const glm::vec3& diffuse)
{
    diffuseColor = glm::vec3(diffuse);

    return *this;
}

Material& Material::AddSpecular(const glm::vec3& specular)
{
    specularColor = glm::vec3(specular);

    return *this;
}

Material& Material::AddShininess(GLfloat shininess)
{
    this->shininess = shininess;

    return *this;
}

Material& Material::AddTexture(const std::string& textureId)
{
    texture = ResourceManager::GetInstance()->GetTexture(textureId);

    return *this;
}

Shader& Material::GetShader() const
{
    return *shader;
}
