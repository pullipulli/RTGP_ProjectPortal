//
// Created by Andrea Pullia on 17/06/2026.
//

#include "Core/Graphics/Material.h"

#include "Core/Graphics/ResourceManager.h"
#include "Core/Graphics/Shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Core/Graphics/PointLight.h"
#include "Core/Graphics/Texture/Texture.h"

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

    shader->SetUniformParameter("currentLightNumber", static_cast<int>(PointLight::lights.size()));

    std::string lightStringPrefix = "lights[";

    for (int i = 0; i < PointLight::lights.size(); i++)
    {
        shader->SetUniformParameter(lightStringPrefix + std::to_string(i) + "].ambient", &PointLight::lights[i]->ambientColor);
        shader->SetUniformParameter(lightStringPrefix + std::to_string(i) + "].diffuse", &PointLight::lights[i]->diffuseColor);
        shader->SetUniformParameter(lightStringPrefix + std::to_string(i) + "].specular", &PointLight::lights[i]->specularColor);
        shader->SetUniformParameter(lightStringPrefix + std::to_string(i) + "].Ka", PointLight::lights[i]->Ka);
        shader->SetUniformParameter(lightStringPrefix + std::to_string(i) + "].Kd", PointLight::lights[i]->Kd);
        shader->SetUniformParameter(lightStringPrefix + std::to_string(i) + "].Ks", PointLight::lights[i]->Ks);
        shader->SetUniformParameter(lightStringPrefix + std::to_string(i) + "].position", &PointLight::lights[i]->position);
    }

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
