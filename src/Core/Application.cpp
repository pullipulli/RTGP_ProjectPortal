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
#include "Core/Graphics/Texture/RenderTexture.h"
#include "Core/Graphics/Material.h"
#include "Core/Graphics/PointLight.h"
#include "Core/Graphics/ResourceManager.h"
#include "Core/SceneObject.h"
#include "glm/gtc/matrix_inverse.hpp"
#include "Core/Camera.h"
#include "Core/Graphics/Model/Model.h"
#include "Core/Physics.h"
#include "Game/Portal.h"

#ifdef _WINDOWS_
    #error windows.h was included!
#endif

#include <iostream>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <Core/Graphics/Shader.h>

Application::~Application() = default;

Application::Application(float screenWidth, float screenHeight)
: screenWidth(screenWidth), screenHeight(screenHeight)
{
    this->camera = std::make_unique<Camera>(glm::vec3(0,0,7), GL_TRUE, 45.f, screenWidth/screenHeight, 0.1f, 100000.f);
}

void Application::StartApplication()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(static_cast<int>(screenWidth), static_cast<int>(screenHeight), "Portal Project", nullptr, nullptr);
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
    RenderTexture* bluePortalRenderTexture = resourceManager->InitializeRenderTexture("BluePortalTexture", 512, 256);
    RenderTexture* redPortalRenderTexture = resourceManager->InitializeRenderTexture("RedPortalTexture", 512, 256);

    Shader* globalShader = resourceManager->InitializeShader("GlobalShader",
        "shaders/vertex/globalShader.vert", "shaders/fragment/globalShader.frag"
    );

    Model* cubeModel = resourceManager->InitializeModel("../assets/models/cube.obj");     // I will use a scaled cube to simulate the static floor/plane
    Model* portalModel = resourceManager->InitializeModel("../assets/models/portal.obj");

    Texture* floorTexture = resourceManager->InitializeTexture("../assets/textures/raw_wooden_floor_01_diff_4k.jpg", GL_REPEAT, GL_NEAREST, GL_LINEAR, true);

    Material* planeMaterial = &Material::Create(globalShader->GetShaderId())
                        .AddDiffuse(glm::vec3(1, 1, 1))
                        .AddSpecular(glm::vec3(0, 0, 0))
                        .AddShininess(1.f)
                        .AddTexture(floorTexture->GetTextureResourceId());

    Material* bluePortalMaterial = &Material::Create(globalShader->GetShaderId())
                    .AddDiffuse(glm::vec3(1, 1, 1))
                    .AddSpecular(glm::vec3(0, 0, 0))
                    .AddShininess(1.f)
                    .AddTexture(bluePortalRenderTexture->GetTextureResourceId());

    Material* redPortalMaterial = &Material::Create(globalShader->GetShaderId())
                .AddDiffuse(glm::vec3(1, 1, 1))
                .AddSpecular(glm::vec3(0, 0, 0))
                .AddShininess(1.f)
                .AddTexture(redPortalRenderTexture->GetTextureResourceId());

    Camera behindRedPortalCamera = Camera{glm::vec3(0,5,0), GL_FALSE, 45.f, screenWidth/screenHeight, 0.1f, 100000.f};
    behindRedPortalCamera.UpdateRotation(0, 90.f, 0);

    Camera behindBluePortalCamera = Camera{glm::vec3(0,5,0), GL_FALSE, 45.f, screenWidth/screenHeight, 0.1f, 100000.f};
    behindBluePortalCamera.UpdateRotation(0, -90.f, 0);

    bluePortal = make_unique<Portal>(
        glm::vec3(-10, 1, 0),
        glm::vec3(90, 0, 0),
        glm::vec3(.8f),
        bluePortalMaterial,
        portalModel->GetModelId(),
        false,
        &behindBluePortalCamera
    );

    redPortal = make_unique<Portal>(
        glm::vec3(-3, 1, 0),
        glm::vec3(90, 0, 0),
        glm::vec3(.8f),
        redPortalMaterial,
        portalModel->GetModelId(),
        false,
        &behindRedPortalCamera
    );

    bluePortal->LinkOtherPortal(redPortal.get());
    redPortal->LinkOtherPortal(bluePortal.get());

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
        currentFrameTime = static_cast<float>(glfwGetTime());
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

        bluePortal->GetRenderTexture()->BindFrameBuffer();
        DrawScene(bluePortal->GetThisPortalCamera(), RenderPass::RenderTexture);

        redPortal->GetRenderTexture()->BindFrameBuffer();
        DrawScene(redPortal->GetThisPortalCamera(), RenderPass::RenderTexture);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, static_cast<int>(screenWidth), static_cast<int>(screenHeight));
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DrawScene(camera.get(), RenderPass::Screen);

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

void Application::DrawScene(const Camera* renderingCamera, const RenderPass currentRenderPass) const
{
    for (SceneObject* object : SceneObject::GetAllActiveSceneObjects())
    {
        object->Render(renderingCamera->GetViewMatrix(), renderingCamera->GetProjectionMatrix(), currentRenderPass);
    }

    if (currentRenderPass == RenderPass::Screen)
    {
        for (SceneObject* object : SceneObject::GetAllActiveSceneObjects())
        {
            object->Update(deltaTime);
        }
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
