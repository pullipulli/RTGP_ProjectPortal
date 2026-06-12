//
// Created by Andrea Pullia on 17/02/2026.
//

#pragma once
#include "glm/vec2.hpp"

typedef float GLfloat;
struct GLFWwindow;

class Input
{
public:
    Input(GLFWwindow *currentWindow, bool shouldCloseWindowOnEsc);
    glm::vec2 GetMouseDelta();
    bool IsKeyPressed(int keyCode) const;
    void UpdateInputEvents();
private:
    // we need to store the previous mouse position to calculate the offset with the current frame
    GLfloat lastMouseX;
    GLfloat lastMouseY;
    // when rendering the first frame, we do not have a "previous state" for the mouse, so we need to manage this situation
    bool firstMouse = true;
    bool shouldCloseWindowOnEsc;
    GLFWwindow *window;
};
