#pragma once

#include "VulkanEnv.h"
#include <vulkan/vulkan.hpp>

class VulkanSwapchain{
private:
    vk::SurfaceKHR vkSurface;
    vk::SwapchainKHR vkSwapchain;
};
