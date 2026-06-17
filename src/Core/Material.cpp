//
// Created by Andrea Pullia on 17/06/2026.
//

#include "Core/Material.h"

#include "Core/ResourceManager.h"
#include "utils/shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

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
    shader->SetUniformParameter("diffuseColor", &diffuseColor);
    shader->SetUniformParameter("specularColor", &specularColor);
    shader->SetUniformParameter("Ka", Ka);
    shader->SetUniformParameter("Kd", Kd);
    shader->SetUniformParameter("Ks", Ks);
    shader->SetUniformParameter("shininess", shininess);
    shader->SetUniformParameter("hasTexture", texture != nullptr);

    if (texture)
        texture->BindTexture();
    shader->Use();
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

Material& Material::AddKa(GLfloat Ka)
{
    this->Ka = Ka;

    return *this;
}

Material& Material::AddKd(GLfloat Kd)
{
    this->Kd = Kd;

    return *this;
}

Material& Material::AddKs(GLfloat Ks)
{
    this->Ks = Ks;

    return *this;
}

Material& Material::AddTexture(const std::string& textureId)
{
    texture = ResourceManager::GetInstance()->GetTexture(textureId);

    return *this;
}