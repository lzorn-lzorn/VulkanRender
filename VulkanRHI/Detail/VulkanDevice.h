#pragma once
#include <vulkan/vulkan.hpp>
#include "../Include/VulkanEnv.h"
#include "vulkan/vulkan_handles.hpp"
class VulkanEnv;
class VulkanDevice{
public:
    VulkanDevice(VulkanEnv, vk::SurfaceKHR);
    ~VulkanDevice();
public:
    unsigned long long GetGPUNum() {
        return mVkEnv.VkInstance().enumeratePhysicalDevices().size();
    }

    vk::PhysicalDevice GetPhysicalDevice() const {
        return mVkPhysicalDevice;
    }
    vk::Device& GetVkDevice() {
        return mVkDevice;
    }
private:
    void CreateDevice();
private:
	VulkanEnv& mVkEnv;
    vk::Device mVkDevice;
    vk::PhysicalDevice mVkPhysicalDevice;
};
// 李卓然