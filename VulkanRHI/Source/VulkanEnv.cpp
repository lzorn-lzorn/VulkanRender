#include "../Include/VulkanEnv.h"
#include <atomic>

// ! VulkanEnv =================================================================
VulkanEnv::VulkanEnv() {
    vkInstance = CreateVulkanInstance();
}

VulkanEnv::~VulkanEnv() {
    // Cleanup Vulkan instance here
    vkInstance.destroy();
}

vk::Instance VulkanEnv::CreateVulkanInstance(){
    // 设置应用程序信息
    vk::ApplicationInfo vkAppInfo{};
    vkAppInfo.pApplicationName = "Vulkan Application";
    vkAppInfo.applicationVersion = GetCurVulkanAPI().Version();
    vkAppInfo.pEngineName = "Custom Engine";
    vkAppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    vkAppInfo.apiVersion = GetCurVulkanAPI().Version();

    vk::InstanceCreateInfo vkCreateInfo{};
    vkCreateInfo.setPApplicationInfo(&vkAppInfo);
    
    std::vector<const char*> instanceExtensions = {
        VK_KHR_SURFACE_EXTENSION_NAME
    };
    
// 添加平台相关的表面扩展
// #ifdef _WIN32
//  #include <vulkan/vulkan_win32.h>
//  instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
// #elif defined(__linux__)
//      #include <vulkan/vulkan_xlib.h>
//  instanceExtensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
// #elif defined(__APPLE__)
//  #include <vulkan/vulkan_macos.h>
//  instanceExtensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
// #endif
    
    // 添加调试扩展（调试模式）
#ifdef _DEBUG
    instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif
    // https://docs.vulkan.org/refpages/latest/refpages/source/VK_KHR_get_physical_device_properties2.html
    instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    
    // 设置扩展
    vkCreateInfo.setPEnabledExtensionNames(instanceExtensions);
    
#ifdef __APPLE__
    // 如果没有该标志则在 MacOS 上可能无法找到支持 Vulkan 的设备
    vkCreateInfo.setFlags(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR);
#endif
    // 设置验证层（调试模式）
    std::vector<const char*> validationLayers;
#ifdef _DEBUG
    validationLayers.push_back("VK_LAYER_KHRONOS_validation");
    vkCreateInfo.setPEnabledLayerNames(validationLayers);
#endif
    
    // 创建Vulkan实例
    try {
        vk::Instance instance = vk::createInstance(vkCreateInfo);
        return instance;
    } catch (const vk::SystemError& err) {
        throw std::runtime_error(
            std::string("Failed to create Vulkan instance: ") + err.what()
        );
    }
}
void VulkanEnv::Initialize(){
}
void VulkanEnv::Quit(){
}