//
// Created by Andrea Pullia on 13/06/2026.
//

#include "Core/ResourceManager.h"

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

ResourceManager::ResourceManager()
{
    std::cout << "prova";
}

ResourceManager::~ResourceManager()
{
}
