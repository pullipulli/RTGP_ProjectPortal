# This project contains the source code for my Real-Time Graphics Programming exam

# Portal

## Project Dependencies

1) assimp
2) bullet
3) glad
4) glfw
5) glm
6) imgui
7) stb_image

## TODO LIST

1) ResourceManager class (centralize all meshes, textures and shaders in a single global point and do not need to recharge/recompile them)
2) Material class (contains a pointer to a shader object and a series of instance parameters, using different maps for each uniform parameter!)
3) GameObject class (contains translation, rotation, scale and material as data; defines a begin and update methods to create specific GameObject logic!)
4) Renderer class (contains all the openGL native function calls and render a scene to the screen)
5) Scene class (contains ALL the GameObjects)