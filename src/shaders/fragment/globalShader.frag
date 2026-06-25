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

// light incidence direction (calculated in vertex shader, interpolated by rasterization)
in vec3 lightDir;
// the transformed normal has been calculated per-vertex in the vertex shader
in vec3 vertexNormal;
// vector from fragment to camera (in view coordinate)
in vec3 vertexViewPosition;

in vec2 TexCoordinates;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float Ka;
    float Kd;
    float Ks;
    bool hasTexture;
};

uniform Material material;

uniform sampler2D ourTexture;

// blinnPhong shading
void main()
{
    // ambient component can be calculated at the beginning
    vec3 color = material.Ka * material.ambient;

    vec3 lightNormalizedDirection = normalize(lightDir);

    vec3 normalizedVertexNormal = normalize(vertexNormal);

    // Lambert coefficient
    float lambertian = max(dot(lightNormalizedDirection, normalizedVertexNormal), 0.0);

    // if the lambert coefficient is positive, then I can calculate the specular component
    if(lambertian > 0.0)
    {
        // the view vector has been calculated in the vertex shader, already negated to have direction from the mesh to the camera
        vec3 viewDirection = normalize( vertexViewPosition );

        // in the Blinn-Phong model we do not use the reflection vector, but the half vector
        vec3 halfwayDirection = normalize(lightNormalizedDirection + viewDirection);

        // we use H to calculate the specular component
        float specAngle = max(dot(halfwayDirection, normalizedVertexNormal), 0.0);
        // shininess application to the specular component
        float specular = pow(specAngle, material.shininess);

        // We add diffusive and specular components to the final color
        // N.B. ): in this implementation, the sum of the components can be different than 1
        color += vec3( material.Kd * lambertian * material.diffuse +
        material.Ks * specular * material.specular);
    }
    colorFrag = vec4(color,1.0);
    if (material.hasTexture)
    {
        colorFrag *= texture(ourTexture, TexCoordinates);
    }
}
