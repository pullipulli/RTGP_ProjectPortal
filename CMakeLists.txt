cmake_minimum_required(VERSION 3.24)
project(RTGPProject_Portal)

set(CMAKE_CXX_STANDARD 17)
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
    NAMES assimp-vc143-mt
    PATHS ${ASSIMP_ROOT}/lib/x64
        ${ASSIMP_ROOT}/lib
    NO_DEFAULT_PATH
)

if(NOT ASSIMP_LIB)
    message(FATAL_ERROR "ERRORE: Non trovo assimp-vc143-mt.lib in libs/assimp/lib/x64. Controlla dove hai copiato i file!")
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

add_executable(RTGPProject_Portal src/main.cpp include ${IMGUI_SOURCES} ${IMGUI_BACKENDS})

target_include_directories(RTGPProject_Portal PUBLIC
        libs/glm
        libs/stb_image
        libs/glad/include
        libs/glfw/include
        libs/imgui
        libs/imgui/backends
        ${ASSIMP_ROOT}/include
        libs/bullet/src
        include
        src
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