#pragma once
#include <cstdint>
#include <stdexcept>
#include <optional>
#include <memory_resource>
#include "../Detail/VulkanTools.h"
#include "vulkan/vulkan_handles.hpp"
#include <vulkan/vulkan.hpp>

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
class VulkanEnv;


class VulkanEnv final {
public:
    static VulkanEnv& Instance() {
        static VulkanEnv envInstance;
        return envInstance;
    }
    static void Initialize();
    static void Quit();

    vk::Instance& VkInstance() { return vkInstance; }
    std::string Name() const { return "Vulkan"; }
    ~VulkanEnv();

    vk::PhysicalDevice GetPhysicalDevice() {
        return curPhysicalDevice;
    }
private:
    VulkanEnv();
    vk::Instance CreateVulkanInstance();
private:
    vk::Instance vkInstance;
    vk::PhysicalDevice curPhysicalDevice;
};

