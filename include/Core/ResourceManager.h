//
// Created by Andrea Pullia on 13/06/2026.
//

#pragma once
#include <mutex>
#include <unordered_map>

class RenderTexture;
class Texture;
class Shader;
class Model;
typedef int GLint;

class ResourceManager
{
public:
    ResourceManager(ResourceManager &other) = delete;
    void operator=(const ResourceManager& other) = delete;

    static ResourceManager* GetInstance();

    /// Tries to create and put a new Model into the map; if it already exists then it just returns the old object
    Model* InitializeModel(const std::string& pathKey);
    /// Tries to create and put a new Shader into the map; if it already exists then it just returns the old object
    Shader* InitializeShader(const std::string& shaderNameKey, const std::string& vertexPath, const std::string& fragmentShader);
    /// Tries to create and put a new Texture into the map; if it already exists then it just returns the old object
    Texture* InitializeTexture(const std::string& pathKey, GLint TextureWrapMode, GLint MinifyingFilter, GLint MagnifyingFilter, bool ShouldGenerateMipMap = false);
    /// Tries to create and put a new RenderTexture into the map; if it already exists then it just returns the old object
    RenderTexture* InitializeRenderTexture(const std::string& key, GLint width, GLint height);

    Model* GetModel(const std::string& key);
    Shader* GetShader(const std::string& key);
    Texture* GetTexture(const std::string& key);
    RenderTexture* GetRenderTexture(const std::string& key);
protected:
    ResourceManager();
    ~ResourceManager() = default;
private:
    static ResourceManager* instance;
    static std::mutex mutex;

    std::unordered_map<std::string, Model> models{};
    std::unordered_map<std::string, Shader> shaders{};
    std::unordered_map<std::string, Texture> textures{};
    std::unordered_map<std::string, RenderTexture> renderTextures{};
};
