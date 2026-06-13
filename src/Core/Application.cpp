//
// Created by Andrea Pullia on 17/02/2026.
//

#include "Core/Application.h"

#ifdef _WIN32
    #define APIENTRY __stdcall
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core/Input.h"
#include "Core/RenderTexture.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "Core/ResourceManager.h"
#include "glm/gtc/matrix_inverse.hpp"
#include "utils/camera.h"
#include "utils/model.h"
#include "utils/physics.h"

#ifdef _WINDOWS_
    #error windows.h was included!
#endif

#include <iostream>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <utils/shader.h>


Application::Application(float screenWidth, float screenHeight)
: screenWidth(screenWidth), screenHeight(screenHeight)
{
    this->camera = new Camera(glm::vec3(0,0,7), GL_TRUE, 45.f, screenWidth/screenHeight, 0.1f, 100000.f);
    this->bulletSimulation = new Physics();
    this->ambientColor = glm::vec3(1.f, 1.f, 1.f);
}

void Application::StartApplication()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Portal Project", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    input = new Input(window, GL_TRUE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    renderTexture = new RenderTexture{512, 256};

    globalShader = new Shader("shaders/vertex/globalShader.vert", "shaders/fragment/globalShader.frag");

    cubeModel = new Model("../assets/models/cube.obj", *globalShader);     // I will use a scaled cube to simulate the static floor/plane
    portalModel = new Model("../assets/models/portal.obj", *globalShader);

    ResourceManager::GetInstance();

    while(!glfwWindowShouldClose(window))
    {
        currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // Check if an I/O event is happening
        glfwPollEvents();

        input->UpdateInputEvents();

        glm::vec2 currentMouseDelta = input->GetMouseDelta();

        camera->ProcessMouseMovement(currentMouseDelta.x, currentMouseDelta.y);

        ApplyCameraMovements();

        // TODO Implement real GUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        renderTexture->BindFrameBuffer();
        DrawScene(RenderPass::RenderTexture);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, screenWidth, screenHeight);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DrawScene(RenderPass::Screen);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        bulletSimulation->dynamicsWorld->stepSimulation((deltaTime < this->FIXED_DELTA_TIME ? deltaTime : this->FIXED_DELTA_TIME), 10);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swapping back and front buffers
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    bulletSimulation->Clear();

    glfwTerminate();
}

void Application::ApplyCameraMovements()
{
    GLboolean diagonal_movement = (input->IsKeyPressed(GLFW_KEY_W) ^ input->IsKeyPressed(GLFW_KEY_S)) && (input->IsKeyPressed(GLFW_KEY_A) ^ input->IsKeyPressed(GLFW_KEY_D));
    camera->SetMovementCompensation(diagonal_movement);

    if(input->IsKeyPressed(GLFW_KEY_W))
        camera->ProcessKeyboard(FORWARD, deltaTime);
    if(input->IsKeyPressed(GLFW_KEY_S))
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    if(input->IsKeyPressed(GLFW_KEY_A))
        camera->ProcessKeyboard(LEFT, deltaTime);
    if(input->IsKeyPressed(GLFW_KEY_D))
        camera->ProcessKeyboard(RIGHT, deltaTime);
}

void Application::DrawScene(RenderPass renderPass)
{
    glm::vec3 plane_pos = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 plane_size = glm::vec3(200.0f, 0.1f, 200.0f);
    glm::vec3 plane_rot = glm::vec3(0.0f, 0.0f, 0.0f);

    btRigidBody* plane = bulletSimulation->createRigidBody(BOX,plane_pos,plane_size,plane_rot,0.0f,0.3f,0.3f);
    glm::mat4 planeModelMatrix = glm::mat4(1.0f);
    glm::mat3 planeNormalMatrix = glm::mat3(1.0f);

    // Model and Normal transformation matrices for the objects in the scene: we set to identity
    glm::mat4 portalModelMatrix = glm::mat4(1.0f);
    glm::mat3 portalNormalMatrix = glm::mat3(1.0f);

    glm::vec3 diffuseColor = glm::vec3(0, 1, 0);
    glm::vec3 specularColor = glm::vec3(0, 0, 0);
    GLfloat Ka = .05f;
    GLfloat Kd = .9;
    GLfloat Ks = .0f;
    GLfloat shininess = 1.f;
    glm::mat4 projMatrix = camera->GetProjectionMatrix();
    glm::mat4 viewMatrix = camera->GetViewMatrix();

    cubeModel->UseShader();

    planeModelMatrix = glm::mat4(1.0f);
    planeNormalMatrix = glm::mat3(1.0f);
    planeModelMatrix = glm::translate(planeModelMatrix, plane_pos);
    planeModelMatrix = glm::scale(planeModelMatrix, plane_size);
    planeNormalMatrix = glm::inverseTranspose(glm::mat3(viewMatrix*planeModelMatrix));

    // "global" uniforms, equal for all the objects in the "scene"
    cubeModel->SetShaderUniformParameter("projectionMatrix", &projMatrix);
    cubeModel->SetShaderUniformParameter("viewMatrix", &viewMatrix);
    cubeModel->SetShaderUniformParameter("pointLightPosition", &lightPos0);
    cubeModel->SetShaderUniformParameter("ambientColor", &ambientColor);


    // "local" uniforms; they depend on the single object
    cubeModel->SetShaderUniformParameter("modelMatrix", &planeModelMatrix);
    cubeModel->SetShaderUniformParameter("normalMatrix", &planeNormalMatrix);
    cubeModel->SetShaderUniformParameter("diffuseColor", &diffuseColor);
    cubeModel->SetShaderUniformParameter("specularColor", &specularColor);
    cubeModel->SetShaderUniformParameter("Ka", Ka);
    cubeModel->SetShaderUniformParameter("Kd", Kd);
    cubeModel->SetShaderUniformParameter("Ks", Ks);
    cubeModel->SetShaderUniformParameter("shininess", shininess);
    cubeModel->SetShaderUniformParameter("hasTexture", false);

    cubeModel->Draw();

    if (renderPass == RenderPass::Screen)
    {
        renderTexture->BindTexture();
        portalModel->UseShader();

        portalModel->SetShaderUniformParameter("projectionMatrix", &projMatrix);
        portalModel->SetShaderUniformParameter("viewMatrix", &viewMatrix);
        portalModel->SetShaderUniformParameter("pointLightPosition", &lightPos0);
        portalModel->SetShaderUniformParameter("ambientColor", &ambientColor);

        portalModelMatrix = glm::mat4(1.0f);
        portalNormalMatrix = glm::mat3(1.0f);
        portalModelMatrix = glm::translate(portalModelMatrix, glm::vec3(-3.0f, 1.0f, 0.0f));
        portalModelMatrix = glm::rotate(portalModelMatrix, glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));
        portalModelMatrix = glm::scale(portalModelMatrix, glm::vec3(0.8f, 0.8f, 0.8f));	// It's a bit too big for our scene, so scale it down
        // if we cast a mat4 to a mat3, we are automatically considering the upper left 3x3 submatrix
        portalNormalMatrix = glm::inverseTranspose(glm::mat3(camera->GetViewMatrix()*portalModelMatrix));
        portalModel->SetShaderUniformParameter("modelMatrix", &portalModelMatrix);
        portalModel->SetShaderUniformParameter("normalMatrix", &portalNormalMatrix);
        portalModel->SetShaderUniformParameter("diffuseColor", &diffuseColor);
        portalModel->SetShaderUniformParameter("specularColor", &specularColor);
        portalModel->SetShaderUniformParameter("Ka", Ka);
        portalModel->SetShaderUniformParameter("Kd", Kd);
        portalModel->SetShaderUniformParameter("Ks", Ks);
        portalModel->SetShaderUniformParameter("shininess", shininess);
        portalModel->SetShaderUniformParameter("hasTexture", true);

        portalModel->Draw();
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
