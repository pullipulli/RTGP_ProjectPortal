/*
Camera class
- creation of a reference system for the camera
- management of camera movement (FPS-style) using WASD keys and mouse movement

N.B.) adaptation of https://github.com/JoeyDeVries/LearnOpenGL/blob/master/includes/learnopengl/camera.h

author: Davide Gadia

Real-time Graphics Programming - a.a. 2024/2025
Master degree in Computer Science
Universita' degli Studi di Milano
*/

#pragma once

// we use GLM to create the view matrix and to manage camera transformations
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// possible camera movements
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera settings
// Initial camera orientation on Y and X (Z not considered)
const GLfloat YAW        = -90.0f; //Y
const GLfloat PITCH      =  0.0f; //X

// speed of camera movement
const GLfloat SPEED      =  3.0f;
// speed compensation in case of diagonal camera movement (= 1/sqrt(2))
const GLfloat DIAGONAL_COMPENSATION = 0.70710678f;
// parameter to weight mouse movement
const GLfloat SENSITIVITY =  0.25f;

///////////////////  CAMERA class ///////////////////////
class Camera
{
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 WorldFront;
    glm::vec3 Up; // camera local UP vector
    glm::vec3 Right;
    glm::vec3 WorldUp; //  camera world UP vector -> needed for the initial computation of Right vector
    GLboolean onGround; // it defines if the camera is "anchored" to the ground, or if it strictly follows the current Front direction (even if this means that the camera "flies" in the scene)
    // N.B.) this version works only for flat terrains
    // Eular Angles
    GLfloat Yaw;
    GLfloat Pitch;
    // Camera speed parameters
    GLfloat MovementSpeed;
    GLfloat MovementCompensation = 1.0f;
    // Camera rotation parameter
    GLfloat MouseSensitivity;
    GLfloat FOV = 45.0f;
    GLfloat aspectRatio = 16.f/9.f;
    GLfloat nearPlane = 0.1f;
    GLfloat farPlane = 10000.0f;

    //////////////////////////////////////////
    // simplified constructor
    // it can be extended passing different values of speed and mouse sensitivity, etc...
    Camera(glm::vec3 position, GLboolean onGround);
    Camera(glm::vec3 position, GLboolean onGround, float FOV, float aspectRatio, float nearPlaneZ, float farPlaneZ, float mouseSensitivity = SENSITIVITY);

    //////////////////////////////////////////
    // it returns the current view matrix
    glm::mat4 GetViewMatrix() const;

    glm::mat4 GetProjectionMatrix() const;

    //////////////////////////////////////////
    // if a single WASD key is pressed, then we will apply the full value of velocity v in the corresponding direction.
    // However, if two keys are pressed together in order to move diagonally (W+D, W+A, S+D, S+A), 
    // then we will apply a compensated velocity in the single directions, in order to have the full v applied in the diagonal direction
    // The compensation is = 1/sqrt(2)
    void SetMovementCompensation(GLboolean diagonal_movement);

    //////////////////////////////////////////
    // it updates camera position when a WASD key is pressed
    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime);

    //////////////////////////////////////////
    // it updates camera orientation when mouse is moved
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch = GL_TRUE);

    void UpdateRotation(float Yaw, float Pitch, float Roll);
    void UpdatePosition(const glm::vec3& position);

private:
    //////////////////////////////////////////
    // it updates the camera reference system
    void UpdateCameraVectors();
};
