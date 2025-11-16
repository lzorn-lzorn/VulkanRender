#pragma once
#include <vulkan/vulkan.hpp>
#include "../Include/VulkanEnv.h"
class VulkanEnv;
class VulkanDevice{
public:
    VulkanDevice(VulkanEnv*, vk::PhysicalDevice*);
    ~VulkanDevice();
public:
    unsigned long long GetGPUNum() {
        return vkEnv->VkInstance().enumeratePhysicalDevices().size();
    }
private:
    void CreateDevice();
private:
	VulkanEnv * vkEnv;
    vk::Device device;
    vk::PhysicalDevice* ptrCurPhysicalDevice{nullptr};
};