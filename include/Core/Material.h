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
    Material& AddDiffuse(const glm::vec3& diffuse);
    Material& AddSpecular(const glm::vec3& specular);
    Material& AddShininess(GLfloat shininess);
    Material& AddKa(GLfloat Ka);
    Material& AddKd(GLfloat Kd);
    Material& AddKs(GLfloat Ks);
    Material& AddTexture(const std::string& textureId);

    Material() = delete;
private:
    glm::vec3 diffuseColor{0};
    glm::vec3 specularColor{0};
    GLfloat shininess{1.f};
    GLfloat Ka{.5f};
    GLfloat Kd{.5f};
    GLfloat Ks{.5f};

    Texture* texture{nullptr};
    Shader* shader;

    explicit Material(const std::string& shaderId);
};