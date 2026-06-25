//
// Created by Andrea Pullia on 25/06/2026.
//

#pragma once

#include "glm/glm.hpp"
#include <vector>

typedef float GLfloat;

struct PointLight
{
    glm::vec3 position{0};
    glm::vec3 ambientColor{1};
    glm::vec3 diffuseColor{0};
    glm::vec3 specularColor{0};
    GLfloat Ka{.5f};
    GLfloat Kd{.5f};
    GLfloat Ks{.5f};

    static std::vector<PointLight*> lights;

    PointLight()
    {
        lights.push_back(this);
    }

    // TODO handle light deactivation/destroy
};