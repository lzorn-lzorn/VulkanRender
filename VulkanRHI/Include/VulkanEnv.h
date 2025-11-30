#pragma once

#include "../Detail/VulkanTools.h"
#include "vulkan/vulkan_handles.hpp"
#include <vulkan/vulkan.hpp>

#include <cassert>
// 平台相关的头文件
// #ifdef _WIN32
// #include <vulkan/vulkan_win32.h>
// #elif defined(__linux__)
// #include <vulkan/vulkan_xlib.h>
// #elif defined(__APPLE__)
// #include <vulkan/vulkan_macos.h>
// #endif

class VulkanQueue;
class VulkanDevice;
class VulkanViewport;

static vk::PhysicalDevice SelectPhysicalDevice(const vk::Instance& instance);
class VulkanEnv final {
public:
    static VulkanEnv& Get() {
        assert(sVkEnv != nullptr && "VulkanEnv is not initialized, call VulkanEnv::Init() first.");
        return *sVkEnv;
    }

    /**
    * @brief 初始化 Vulkan 环境, 分配内存
    * @note 如果是封装多个 RHI, 可以考虑不使用 static
    */
    static void Init();
    
    /**
    * @brief 销毁 Vulkan 环境, 释放内存
    */
    static void Shutdown();

    vk::Instance& VkInstance() { return vkInstance; }
    std::string Name() const { return "Vulkan"; }

    VulkanEnv();
    ~VulkanEnv();
    
private:
    
    void CreateVulkanInstance();
    void CreateVulkanDevice();
private:
    static VulkanEnv* sVkEnv;
    vk::Instance vkInstance;
    VulkanDevice* vkDevice {nullptr};
};


