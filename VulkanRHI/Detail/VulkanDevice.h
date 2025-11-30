#pragma once
#include <vulkan/vulkan.hpp>
#include "vulkan/vulkan_handles.hpp"
#include <vector>

class VulkanEnv;

class VulkanDevice{
public:
    VulkanDevice(VulkanEnv*, vk::PhysicalDevice);
    ~VulkanDevice();
public:


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
    vk::PhysicalDeviceProperties mVkPhysicalDeviceProperties;
    std::vector <const char*> mDeviceExtensions;
};