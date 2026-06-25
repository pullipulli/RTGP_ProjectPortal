//
// Created by Andrea Pullia on 17/06/2026.
//

#pragma once
#include <string>

#include "glm/glm.hpp"

class Shader;
class Texture;
typedef float GLfloat;

class Material
{
public:
    static Material Create(const std::string& shaderId);
    void Use() const;
    Material& AddAmbient(const glm::vec3& ambient);
    Material& AddDiffuse(const glm::vec3& diffuse);
    Material& AddSpecular(const glm::vec3& specular);
    Material& AddShininess(GLfloat shininess);
    Material& AddTexture(const std::string& textureId);

    Shader& GetShader() const;

    Material() = delete;
private:
    glm::vec3 ambientColor{1};
    glm::vec3 diffuseColor{0};
    glm::vec3 specularColor{0};
    GLfloat shininess{1.f};

    Texture* texture{nullptr};
    Shader* shader;

    explicit Material(const std::string& shaderId);
};