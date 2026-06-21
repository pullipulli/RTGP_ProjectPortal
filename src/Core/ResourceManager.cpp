//
// Created by Andrea Pullia on 13/06/2026.
//

#include "Core/ResourceManager.h"
#include "utils/model.h"
#include "utils/shader.h"
#include "Core/Texture.h"
#include "Core/RenderTexture.h"
#include <glad/glad.h>
#include <iostream>

ResourceManager* ResourceManager::instance{nullptr};
std::mutex ResourceManager::mutex{};

ResourceManager * ResourceManager::GetInstance()
{
    std::lock_guard lock(mutex);

    if (instance == nullptr)
    {
        instance = new ResourceManager();
    }

    return instance;
}

Model* ResourceManager::InitializeModel(const std::string& pathKey)
{
    auto [pair, inserted] = models.try_emplace(pathKey, pathKey);

    return &pair->second;
}

Shader* ResourceManager::InitializeShader(const std::string& shaderNameKey, const std::string& vertexPath, const std::string& fragmentShader)
{
    auto [pair, inserted] = shaders.try_emplace(shaderNameKey, shaderNameKey, vertexPath.c_str(), fragmentShader.c_str());

    return &pair->second;
}

Texture* ResourceManager::InitializeTexture(const std::string& pathKey, GLint TextureWrapMode, GLint MinifyingFilter, GLint MagnifyingFilter, bool ShouldGenerateMipMap)
{
    auto [pair, inserted] = textures.try_emplace(pathKey,
        &pathKey, TextureWrapMode, MinifyingFilter, MagnifyingFilter, ShouldGenerateMipMap
        );

    return &pair->second;
}

RenderTexture* ResourceManager::InitializeRenderTexture(const std::string& key, GLint width, GLint height)
{
    auto [pair, inserted] = renderTextures.try_emplace(key, width, height, key);

    return &pair->second;
}

Model* ResourceManager::GetModel(const std::string& key)
{
    const auto pair = models.find(key);

    if (pair != models.end())   // not found model with this key
        return &pair->second;

    return nullptr;
}

Shader* ResourceManager::GetShader(const std::string& key)
{
    const auto pair = shaders.find(key);

    if (pair != shaders.end())   // not found shader with this key
        return &pair->second;

    return nullptr;
}

Texture* ResourceManager::GetTexture(const std::string& key)
{
    // first search against RenderTextures: they are probably less than the Textures
    const auto renderTexturePair = renderTextures.find(key);

    if (renderTexturePair != renderTextures.end())   // not found renderTexture with this key
        return &renderTexturePair->second;

    // then if no RenderTextures match search against the Textures
    const auto texturePair = textures.find(key);

    if (texturePair != textures.end())   // not found texture with this key
        return &texturePair->second;

    return nullptr;
}

RenderTexture* ResourceManager::GetRenderTexture(const std::string &key)
{
    const auto pair = renderTextures.find(key);

    if (pair != renderTextures.end())   // not found texture with this key
        return &pair->second;

    return nullptr;
}

std::vector<RenderTexture*> ResourceManager::GetAllRenderTextures()
{
    std::vector<RenderTexture*> out{};
    out.reserve(renderTextures.size());

    for (auto [_, value] : renderTextures)
        {
            out.push_back(&value);
        }

    return out;
}

ResourceManager::ResourceManager()
{
    std::cout << "Created Singleton ResourceManager!";
}
