/*
00_fullcolor.frag : basic Fragment shader, it applies an uniform color to all the fragments. Color is passed as uniform from the main application

N.B.)  "globalShader.vert" must be used as vertex shader

author: Davide Gadia

Real-Time Graphics Programming - a.a. 2024/2025
Master degree in Computer Science
Universita' degli Studi di Milano

*/

#version 410 core

// output shader variable
out vec4 colorFrag;

// the transformed normal has been calculated per-vertex in the vertex shader
in vec3 vertexNormal;
// vector from fragment to camera (in view coordinate)
in vec3 vertexViewPosition;
in vec3 fragmentPosition;
in vec2 TexCoordinates;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    bool hasTexture;
};

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float Ka;
    float Kd;
    float Ks;
};

#define MAX_LIGHT_NUMBER 150
uniform Light lights[MAX_LIGHT_NUMBER];

uniform Material material;
uniform int currentLightNumber;

uniform sampler2D ourTexture;

vec3 CalculatePointLightColor(Light light, vec3 normalizedVertexNormal, vec3 viewDirection);

// blinnPhong shading
void main()
{
    vec3 color = vec3(0);
    vec3 normalizedVertexNormal = normalize(vertexNormal);
    vec3 viewDirection = normalize(vertexViewPosition);

    for (int i = 0; i < currentLightNumber; i++)
    {
        color += CalculatePointLightColor(lights[i], normalizedVertexNormal, viewDirection);
    }

    colorFrag = vec4(color,1.0);
    if (material.hasTexture)
    {
        colorFrag *= texture(ourTexture, TexCoordinates);
    }
}

vec3 CalculatePointLightColor(Light light, vec3 normalizedVertexNormal, vec3 viewDirection)
{
    // ambient component can be calculated at the beginning
    vec3 color = light.Ka * light.ambient * material.ambient;

    vec3 lightNormalizedDirection = normalize(light.position - fragmentPosition);

    // Lambert coefficient
    float lambertian = max(dot(lightNormalizedDirection, normalizedVertexNormal), 0.0);

    // if the lambert coefficient is positive, then I can calculate the specular component
    if(lambertian > 0.0)
    {
        // in the Blinn-Phong model we do not use the reflection vector, but the half vector
        vec3 halfwayDirection = normalize(lightNormalizedDirection + viewDirection);

        // we use H to calculate the specular component
        float specAngle = max(dot(halfwayDirection, normalizedVertexNormal), 0.0);
        // shininess application to the specular component
        float specular = pow(specAngle, material.shininess);

        // We add diffusive and specular components to the final color
        // N.B. ): in this implementation, the sum of the components can be different than 1
        vec3 totalDiffuse = vec3(light.Kd * lambertian * light.diffuse * material.diffuse);
        vec3 totalSpecular = vec3(light.Ks * specular * light.specular * material.specular);
        color += totalDiffuse + totalSpecular;
    }

    return color;
}