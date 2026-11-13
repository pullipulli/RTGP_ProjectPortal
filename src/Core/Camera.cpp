//
// Created by Andrea Pullia on 28/06/2026.
//

#include "Core/Camera.h"

Camera::Camera(glm::vec3 position, GLboolean onGround)
    :Position(position),onGround(onGround),Yaw(YAW),Pitch(PITCH),MovementSpeed(SPEED),MouseSensitivity(SENSITIVITY)
{
    this->WorldUp = glm::vec3(0.0f,1.0f,0.0f);
    // initialization of the camera reference system
    this->UpdateCameraVectors();
}

Camera::Camera(glm::vec3 position, GLboolean onGround, float FOV, float aspectRatio, float nearPlaneZ, float farPlaneZ, float mouseSensitivity) : Camera(position, onGround)
{
    this->FOV = FOV;
    this->aspectRatio = aspectRatio;
    this->nearPlane = nearPlaneZ;
    this->farPlane = farPlaneZ;
    this->MouseSensitivity = mouseSensitivity;
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    return glm::perspective(this->FOV, this->aspectRatio, this->nearPlane, this->farPlane);
}

void Camera::SetMovementCompensation(GLboolean diagonal_movement)
{
    this->MovementCompensation = (diagonal_movement ? DIAGONAL_COMPENSATION : 1.0f);
}

void Camera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
{
    // the velocity is weighted by a compensation factor
    // = 1 if a single WASD is pressed
    // = 1/sqrt(2) if two keys are pressed for a diagonal movement
    GLfloat velocity = this->MovementSpeed * deltaTime * this->MovementCompensation;

    if (direction == FORWARD)
        this->Position += (this->onGround ? this->WorldFront : this->Front) * velocity;
    if (direction == BACKWARD)
        this->Position -= (this->onGround ? this->WorldFront : this->Front) * velocity;
    if (direction == LEFT)
        this->Position -= this->Right * velocity;
    if (direction == RIGHT)
        this->Position += this->Right * velocity;
}

void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch)
{
    // the sensitivity is applied to weight the movement
    xoffset *= this->MouseSensitivity;
    yoffset *= this->MouseSensitivity;

    // rotation angles on Y and X are updated
    this->Yaw   += xoffset;
    this->Pitch += yoffset;

    // we apply a constraint to the rotation on X, to avoid to have the camera flipped upside down
    // N.B.) this constraint helps to avoid gimbal lock, if all the 3 rotations are considered
    if (constraintPitch)
    {
        if (this->Pitch > 89.0f)
            this->Pitch = 89.0f;
        if (this->Pitch < -89.0f)
            this->Pitch = -89.0f;
    }

    // the camera reference system is updated using the new camera rotations
    this->UpdateCameraVectors();
}

void Camera::UpdateRotation(float Yaw, float Pitch, float Roll)
{
    this->Yaw = Yaw;
    this->Pitch = Pitch;
    this->UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
    // it computes the new Front vector using trigonometric calculations using Yaw and Pitch angles
    // https://learnopengl.com/#!Getting-started/Camera
    glm::vec3 front;
    front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
    front.y = sin(glm::radians(this->Pitch));
    front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
    this->Front = glm::normalize(front);
    // if the camera is "anchored" to the ground, the world Front vector is equal to the local Front vector, but with the y component = 0
    front.y = 0.0f;
    this->WorldFront = glm::normalize(front);
    // Once calculated the new view direction, we re-calculate the Right vector as cross product between Front and world Up vector
    this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
    // we calculate the camera local Up vector as cross product between Front and Right vectors
    this->Up    = glm::normalize(glm::cross(this->Right, this->Front));
}
