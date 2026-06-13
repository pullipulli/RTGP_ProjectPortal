//
// Created by Andrea Pullia on 13/06/2026.
//

#pragma once
#include <mutex>

class ResourceManager
{
public:
    ResourceManager(ResourceManager &other) = delete;
    void operator=(const ResourceManager& other) = delete;

    static ResourceManager* GetInstance();
protected:
    ResourceManager();
    ~ResourceManager();
private:
    static ResourceManager* instance;
    static std::mutex mutex;
};

