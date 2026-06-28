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
#include "Core/Material.h"
#include "Core/PointLight.h"
#include "Core/ResourceManager.h"
#include "Core/SceneObject.h"
#include "glm/gtc/matrix_inverse.hpp"
#include "utils/camera.h"
#include "utils/model.h"
#include "../../include/Core/Physics.h"

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
    this->camera = std::make_unique<Camera>(glm::vec3(0,0,7), GL_TRUE, 45.f, screenWidth/screenHeight, 0.1f, 100000.f);
}

Application::~Application()
{
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

    input = std::make_unique<Input>(window, GL_TRUE);

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
    RenderTexture* portalRenderTexture = resourceManager->InitializeRenderTexture("PortalTexture", 512, 256);

    Shader* globalShader = resourceManager->InitializeShader("GlobalShader",
        "shaders/vertex/globalShader.vert", "shaders/fragment/globalShader.frag"
    );

    Model* cubeModel = resourceManager->InitializeModel("../assets/models/cube.obj");     // I will use a scaled cube to simulate the static floor/plane
    Model* portalModel = resourceManager->InitializeModel("../assets/models/portal.obj");

    Material* planeMaterial = &Material::Create(globalShader->GetShaderId())
                        .AddDiffuse(glm::vec3(0, 1, 0))
                        .AddSpecular(glm::vec3(0, 0, 0))
                        .AddShininess(1.f);

    Material* portalMaterial = &Material::Create(globalShader->GetShaderId())
                    .AddDiffuse(glm::vec3(0, 1, 0))
                    .AddSpecular(glm::vec3(0, 0, 0))
                    .AddShininess(1.f)
                    .AddTexture(portalRenderTexture->GetTextureResourceId());

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

    PointLight pointLight;

    pointLight.position = glm::vec3(-5, 5, 1);
    pointLight.ambientColor = glm::vec3{1};
    pointLight.diffuseColor = glm::vec3{1};
    pointLight.specularColor = glm::vec3{0};
    pointLight.Ka = 0.05f;
    pointLight.Kd = .9f;
    pointLight.Ks = 0;

    PointLight pointLight1;

    pointLight1.position = glm::vec3(5, 5, 1);
    pointLight1.ambientColor = glm::vec3{.5};
    pointLight1.diffuseColor = glm::vec3{1};
    pointLight1.specularColor = glm::vec3{.1};
    pointLight1.Ka = 0.05f;
    pointLight1.Kd = .9f;
    pointLight1.Ks = 0;

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

        for (auto renderTexture: ResourceManager::GetInstance()->GetAllRenderTextures())
        {
            renderTexture->BindFrameBuffer();
            DrawScene(RenderPass::RenderTexture);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, screenWidth, screenHeight);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DrawScene(RenderPass::Screen);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        // TODO move this to a method inside Physics!
        Physics::GetInstance()->dynamicsWorld->stepSimulation((deltaTime < this->FIXED_DELTA_TIME ? deltaTime : this->FIXED_DELTA_TIME), 10);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swapping back and front buffers
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    Physics::GetInstance()->Clear();

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
