cmake_minimum_required(VERSION 3.24)
project(RTGPProject_Portal)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "" FORCE)

add_subdirectory(libs/bullet)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(libs/glfw)

add_library(glad libs/glad/src/glad.c)
target_include_directories(glad PUBLIC libs/glad/include)

set(ASSIMP_ROOT libs/assimp)

find_library(ASSIMP_LIB
    NAMES assimp-vc143-mt.lib
    PATHS ${ASSIMP_ROOT}/lib/x64
        ${ASSIMP_ROOT}/lib
    NO_DEFAULT_PATH
)

find_file(ASSIMP_DLL
    NAMES assimp-vc143-mt.dll
    PATHS ${ASSIMP_ROOT}/bin/x64
        ${ASSIMP_ROOT}/bin
    NO_DEFAULT_PATH
)

if(NOT ASSIMP_LIB)
    message(FATAL_ERROR "Can't find assimp-vc143-mt.lib file")
endif()

if(NOT ASSIMP_DLL)
    message(FATAL_ERROR "Can't find assimp-vc143-mt.dll file")
endif()


set(IMGUI_DIR libs/imgui)

set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
)

set(IMGUI_BACKENDS
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_executable(RTGPProject_Portal
        src/Core/main.cpp include ${IMGUI_SOURCES} ${IMGUI_BACKENDS}
        src/Core/Input.cpp
        src/Core/Application.cpp
        src/Core/RenderTexture.cpp
        src/Core/Texture.cpp
        src/Core/ResourceManager.cpp
        src/Core/Material.cpp
)

target_include_directories(RTGPProject_Portal PUBLIC
        libs
        libs/stb_image
        libs/glad/include
        libs/glfw/include
        libs/imgui
        libs/imgui/backends
        ${ASSIMP_ROOT}/include
        libs/bullet/src
        include
)

target_link_libraries(RTGPProject_Portal
        PRIVATE
        glad
        glfw
        BulletDynamics
        BulletCollision
        LinearMath
        ${ASSIMP_LIB}
        ${CMAKE_DL_LIBS}
)

add_custom_command(TARGET RTGPProject_Portal POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ASSIMP_DLL}"
        "$<TARGET_FILE_DIR:RTGPProject_Portal>"
)