#pragma once

#include "VulkanEnv.h"
#include <vulkan/vulkan.hpp>

class VulkanSwapchain{

public:
    VulkanSwapchain(vk::SurfaceKHR surface, size_t width, size_t height)
        : vkSurface(surface), vkSwapchain(VK_NULL_HANDLE) {}

    ~VulkanSwapchain() = default;
    
private:
    vk::SurfaceKHR vkSurface;
    vk::SwapchainKHR vkSwapchain;
};
