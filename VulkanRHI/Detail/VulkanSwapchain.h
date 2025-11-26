#pragma once
#include "VulkanDevice.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"
#include <vulkan/vulkan.hpp>
#include <vector>

class VulkanDevice;
class VulkanSwapchain{
public:
    VulkanSwapchain(vk::Instance vkInstance, VulkanDevice& device, vk::SurfaceKHR surface, size_t width, size_t height);
    ~VulkanSwapchain() = default;

    std::vector<vk::Image> GetVkImages();
    
private:
    vk::SurfaceKHR mVkSurface;
    vk::SwapchainKHR mVkSwapchain;
    VulkanDevice& mDevice;
    size_t mWidth, mHeight;
    struct SwapChainInfo {
        vk::SurfaceFormatKHR vkFormat;
        vk::Extent2D vkExtent2D;
        std::uint32_t count;
        vk::SurfaceTransformFlagBitsKHR vkTransform;
    } mSwapChainInfo;
private:
    void InitSwapChainInfo(const size_t width, const size_t height);
    vk::SwapchainKHR CreateSwapChain();
    vk::SurfaceFormatKHR QuerySurfaceFormat();
    vk::Extent2D QuerySurfaceExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const size_t width, const size_t height);

};
