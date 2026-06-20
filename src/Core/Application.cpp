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
#include "Core/Material.h"
#include "Core/ResourceManager.h"
#include "Core/SceneObject.h"
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
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    ResourceManager* resourceManager = ResourceManager::GetInstance();
    renderTexture = resourceManager->InitializeRenderTexture("PortalTexture", 512, 256);

    Shader* globalShader = resourceManager->InitializeShader("GlobalShader",
        "shaders/vertex/globalShader.vert", "shaders/fragment/globalShader.frag"
    );

    cubeModel = resourceManager->InitializeModel("../assets/models/cube.obj", globalShader);     // I will use a scaled cube to simulate the static floor/plane
    portalModel = resourceManager->InitializeModel("../assets/models/portal.obj", globalShader);

    planeMaterial = &Material::Create(globalShader->GetShaderId())
                        .AddDiffuse(glm::vec3(0, 1, 0))
                        .AddSpecular(glm::vec3(0, 0, 0))
                        .AddShininess(1.f)
                        .AddKa(.05f)
                        .AddKs(0.f)
                        .AddKd(.9f);

    portalMaterial = &Material::Create(globalShader->GetShaderId())
                    .AddDiffuse(glm::vec3(0, 1, 0))
                    .AddSpecular(glm::vec3(0, 0, 0))
                    .AddShininess(1.f)
                    .AddKa(.05f)
                    .AddKs(0.f)
                    .AddKd(.9f)
                    .AddTexture(renderTexture->GetTextureResourceId());

    SceneObject portalObject{
        glm::vec3(-3, 1, 0),
        glm::vec3(90, 0, 0),
        glm::vec3(.8f),
        portalMaterial,
        portalModel->GetModelId()
    };

    SceneObject planeObject{
        glm::vec3(0, -1, 0),
        glm::vec3(0),
        glm::vec3(200, 0.1f, 200),
        planeMaterial,
        cubeModel->GetModelId()
    };

    //btRigidBody* plane = bulletSimulation->createRigidBody(BOX,plane_pos,plane_size,plane_rot,0.0f,0.3f,0.3f);

    for (SceneObject* object : SceneObject::GetAllActiveSceneObjects())
    {
        object->Start();
    }

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
    for (SceneObject* object : SceneObject::GetAllActiveSceneObjects())
    {
        object->Render(camera->GetViewMatrix(), camera->GetProjectionMatrix(), renderPass);
    }

    if (renderPass == RenderPass::Screen)
    {
        for (SceneObject* object : SceneObject::GetAllActiveSceneObjects())
        {
            object->Update(deltaTime);
        }
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
