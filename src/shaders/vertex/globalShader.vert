/*
globalShader.vert : basic Vertex shader

author: Davide Gadia

Real-Time Graphics Programming - a.a. 2024/2025
Master degree in Computer Science
Universita' degli Studi di Milano

*/


#version 410 core

// vertex position in world coordinates
layout (location = 0) in vec3 position;
// vertex normal in world coordinate
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvCoordinates;

// model matrix
uniform mat4 modelMatrix;
// view matrix
uniform mat4 viewMatrix;
// Projection matrix
uniform mat4 projectionMatrix;

uniform mat3 normalMatrix;

uniform vec3 pointLightPosition;
out vec3 lightDir;

out vec3 vertexNormal;
out vec3 vertexViewPosition;
out vec2 TexCoordinates;

void main()
{
    vec4 modelViewPosition = viewMatrix * modelMatrix * vec4( position, 1.0 );

    vertexViewPosition = -modelViewPosition.xyz;

    vertexNormal = normalize( normalMatrix * normal );

    vec4 lightPos = viewMatrix  * vec4(pointLightPosition, 1.0);
    lightDir = lightPos.xyz - modelViewPosition.xyz;

    gl_Position = projectionMatrix * modelViewPosition;

    TexCoordinates = uvCoordinates;
}
