
#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include "VulkanSupport.h"

/** 
 * @brief 从给定的物理设备列表中选择最佳的物理设备
 * 
 * @param vkPhysicalDevices 可用的Vulkan物理设备列表
 * @return vk::PhysicalDevice 选择的最佳物理设备
 */
vk::PhysicalDevice GetBestPhysicalDevice(const std::vector<vk::PhysicalDevice>& vkPhysicalDevices);

/** 
 * @brief 获取当前引擎支持的 Vulkan API版本
 * 
 * @return VulkanAPI 当前支持的 Vulkan API 信息
 */
VulkanAPI GetCurVulkanAPI();