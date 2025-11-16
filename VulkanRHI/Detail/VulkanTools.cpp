#include "VulkanTools.h"
#include "VulkanSupport.h"
#include <stdexcept>

vk::PhysicalDevice GetBestPhysicalDevice(const std::vector<vk::PhysicalDevice>& vkPhysicalDevices){
    if (vkPhysicalDevices.empty()) {
        throw std::runtime_error("No Vulkan physical devices found.");
    }
    
    // 使用 VulkanDeviceSelector 选择最佳设备
    VulkanDeviceSelector selector;
    
    // 设置必需特性
    selector.RequireCore(VulkanCoreFeature::Graphics)  // 必须支持图形操作
            .RequireCore(VulkanCoreFeature::Compute)   // 必须支持计算操作
            .RequireMemory(VulkanMemoryFeature::DeviceLocal)  // 必须有设备本地内存
            .RequireMemory(VulkanMemoryFeature::HostVisible); // 必须有主机可见内存
    
    // 设置可选特性（这些特性会提高设备得分）
    selector.PreferExtension(VulkanExtensionFeature::Swapchain)  // 优先支持交换链
            .PreferExtension(VulkanExtensionFeature::DynamicRendering)  // 优先支持动态渲染
            .PreferExtension(VulkanExtensionFeature::Synchronization2)  // 优先支持同步2.0
            .PreferExtension(VulkanExtensionFeature::RayTracing)  // 优先支持光线追踪
            .PreferExtension(VulkanExtensionFeature::MeshShader)  // 优先支持网格着色器
            .PreferShader(VulkanShaderFeature::GeometryShader)  // 优先支持几何着色器
            .PreferShader(VulkanShaderFeature::TessellationShader)  // 优先支持曲面细分
            .PreferRender(VulkanRenderFeature::SamplerAnisotropy)  // 优先支持各向异性过滤
            .PreferRender(VulkanRenderFeature::MultiViewport);  // 优先支持多视口
    
    // 设置最低API版本要求
    selector.RequireVersion(GetCurVulkanAPI());
    
    // 优先选择独立GPU
    selector.PreferDiscreteGPU(1000)
            .PreferIntegratedGPU(500);
    
    // 从设备列表中选择最佳设备
    auto selectedDevice = selector.SelectBestDevice(vkPhysicalDevices);
    
    if (!selectedDevice) {
        throw std::runtime_error("No suitable Vulkan device found that meets the requirements.");
    }
    
    // 获取并打印选中设备的能力信息
    auto capabilities = selector.GetSelectedCapabilities();
    if (capabilities) {
        capabilities->PrintCapabilitiesSummary();
    }
    
    return *selectedDevice;
}

VulkanAPI GetCurVulkanAPI() {
    return VulkanAPI(1, 3, 0);
}