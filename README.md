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

1) Position cameras behind the right portals
2) Render other portal camera inside his own RenderTexture

### Other Ideas
- In the material class you should use different maps for each different uniform parameter type!
- Refine SceneObject to make RigidBody customizable when instantiated
- Make the ResourceManager maps use a UNIQUE pointer to the classes instead of a direct reference
- Implement RAII principles for `RenderTexture`, `Texture`, `Material`, and `Shader`