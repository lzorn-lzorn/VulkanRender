#pragma once
#include <vulkan/vulkan.hpp>
#include "../Include/VulkanEnv.h"
class VulkanEnv;
class VulkanDevice{
public:
    VulkanDevice(VulkanEnv*, vk::SurfaceKHR);
    ~VulkanDevice();
public:
    unsigned long long GetGPUNum() {
        return vk_env->VkInstance().enumeratePhysicalDevices().size();
    }
private:
    void CreateDevice();
private:
	VulkanEnv * vk_env;
    vk::Device vk_device;
};
// 李卓然