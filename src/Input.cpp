//
// Created by Andrea Pullia on 17/02/2026.
//

#include "Input.h"

#include "GLFW/glfw3.h"
#include "glm.hpp"

Input::Input(GLFWwindow *currentWindow, bool shouldCloseWindowOnEsc)
: window(currentWindow), shouldCloseWindowOnEsc(shouldCloseWindowOnEsc)
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    UpdateInputEvents();
}

void Input::UpdateInputEvents()
{
    if(IsKeyPressed(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, shouldCloseWindowOnEsc);
}

glm::vec2 Input::GetMouseDelta()
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if(firstMouse)
    {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }

    float deltaMouseX = xpos - lastMouseX;
    float deltaMouseY = lastMouseX - ypos;

    lastMouseX = xpos;
    lastMouseY = ypos;

    glm::vec2(deltaMouseX, deltaMouseY);
    return glm::vec2(deltaMouseX, deltaMouseY);
}

bool Input::IsKeyPressed(int keyCode) const
{
    return glfwGetKey(window, keyCode) != GLFW_RELEASE;
}