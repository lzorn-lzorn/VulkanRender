#include "VulkanSupport.h"
#include <algorithm>
#include <iostream>

// VulkanDeviceCapabilities 实现
void VulkanDeviceCapabilities::QueryFromDevice(vk::PhysicalDevice device) {
    physicalDevice = device;
    
    QueryBasicProperties();
    QueryQueueFamilies();
    QueryExtensions();
    QueryAdvancedFeatures();
    FindQueueFamilies();
}

void VulkanDeviceCapabilities::QueryBasicProperties() {
    properties = physicalDevice.getProperties();
    features = physicalDevice.getFeatures();
    memoryProperties = physicalDevice.getMemoryProperties();
}

void VulkanDeviceCapabilities::QueryQueueFamilies() {
    queueFamilies = physicalDevice.getQueueFamilyProperties();
}

void VulkanDeviceCapabilities::QueryExtensions() {
    availableExtensions = physicalDevice.enumerateDeviceExtensionProperties();
}

void VulkanDeviceCapabilities::QueryAdvancedFeatures() {
    // 查询 Vulkan 1.1+ 特性
    if (properties.apiVersion >= VK_API_VERSION_1_1) {
        vk::PhysicalDeviceFeatures2 features2;
        
        features2.pNext = &vulkan11Features;
        
        if (properties.apiVersion >= VK_API_VERSION_1_2) {
            vulkan11Features.pNext = &vulkan12Features;
            
            if (properties.apiVersion >= VK_API_VERSION_1_3) {
                vulkan12Features.pNext = &vulkan13Features;
            }
        }
        
        physicalDevice.getFeatures2(&features2);
    }
    
    // 查询扩展特性（如果扩展可用）
    if (CheckExtensionSupport(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)) {
        vk::PhysicalDeviceFeatures2 features2;
        features2.pNext = &rayTracingFeatures;
        physicalDevice.getFeatures2(&features2);
    }
    
    if (CheckExtensionSupport(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)) {
        vk::PhysicalDeviceFeatures2 features2;
        features2.pNext = &accelerationStructureFeatures;
        physicalDevice.getFeatures2(&features2);
    }
    
    if (CheckExtensionSupport(VK_EXT_MESH_SHADER_EXTENSION_NAME)) {
        vk::PhysicalDeviceFeatures2 features2;
        features2.pNext = &meshShaderFeatures;
        physicalDevice.getFeatures2(&features2);
    }
    
    // ... 其他扩展特性查询
}

void VulkanDeviceCapabilities::FindQueueFamilies() {
    for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
        const auto& qf = queueFamilies[i];
        
        if (qf.queueFlags & vk::QueueFlagBits::eGraphics && !graphicsQueueFamily) {
            graphicsQueueFamily = i;
        }
        if (qf.queueFlags & vk::QueueFlagBits::eCompute && !computeQueueFamily) {
            computeQueueFamily = i;
        }
        if (qf.queueFlags & vk::QueueFlagBits::eTransfer && !transferQueueFamily) {
            transferQueueFamily = i;
        }
    }
}

bool VulkanDeviceCapabilities::CheckExtensionSupport(const char* extensionName) const {
    return std::any_of(availableExtensions.begin(), availableExtensions.end(),
        [extensionName](const vk::ExtensionProperties& ext) {
            return strcmp(ext.extensionName, extensionName) == 0;
        });
}

bool VulkanDeviceCapabilities::IsSupported(VulkanCoreFeature feature) const {
    switch (feature) {
        case VulkanCoreFeature::Graphics:
            return graphicsQueueFamily.has_value();
        case VulkanCoreFeature::Compute:
            return computeQueueFamily.has_value();
        case VulkanCoreFeature::Transfer:
            return transferQueueFamily.has_value();
        case VulkanCoreFeature::SparseBinding:
            return features.sparseBinding;
        case VulkanCoreFeature::Protected:
            return properties.apiVersion >= VK_API_VERSION_1_1 && 
                   vulkan11Features.protectedMemory;
        default:
            return false;
    }
}

bool VulkanDeviceCapabilities::IsSupported(VulkanExtensionFeature feature) const {
    switch (feature) {
        case VulkanExtensionFeature::Swapchain:
            return CheckExtensionSupport(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        case VulkanExtensionFeature::Surface:
            return true; // Instance level extension
        case VulkanExtensionFeature::RayTracing:
        case VulkanExtensionFeature::RayTracingPipeline:
            return CheckExtensionSupport(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME) &&
                   rayTracingFeatures.rayTracingPipeline;
        case VulkanExtensionFeature::AccelerationStructure:
            return CheckExtensionSupport(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME) &&
                   accelerationStructureFeatures.accelerationStructure;
        case VulkanExtensionFeature::MeshShader:
            return CheckExtensionSupport(VK_EXT_MESH_SHADER_EXTENSION_NAME) &&
                   meshShaderFeatures.meshShader;
        case VulkanExtensionFeature::DynamicRendering:
            return properties.apiVersion >= VK_API_VERSION_1_3 &&
                   vulkan13Features.dynamicRendering;
        case VulkanExtensionFeature::Synchronization2:
            return properties.apiVersion >= VK_API_VERSION_1_3 &&
                   vulkan13Features.synchronization2;
        case VulkanExtensionFeature::TimelineSemaphore:
            return properties.apiVersion >= VK_API_VERSION_1_2 &&
                   vulkan12Features.timelineSemaphore;
        case VulkanExtensionFeature::DebugUtils:
            return CheckExtensionSupport(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        case VulkanExtensionFeature::MemoryBudget:
            return CheckExtensionSupport(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
        case VulkanExtensionFeature::BufferDeviceAddress:
            return vulkan12Features.bufferDeviceAddress;
        case VulkanExtensionFeature::DescriptorIndexing:
            return vulkan12Features.descriptorIndexing;
        default:
            return false;
    }
}

bool VulkanDeviceCapabilities::IsSupported(VulkanMemoryFeature feature) const {
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        const auto& memType = memoryProperties.memoryTypes[i];
        switch (feature) {
            case VulkanMemoryFeature::DeviceLocal:
                if (memType.propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal)
                    return true;
                break;
            case VulkanMemoryFeature::HostVisible:
                if (memType.propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible)
                    return true;
                break;
            case VulkanMemoryFeature::HostCached:
                if (memType.propertyFlags & vk::MemoryPropertyFlagBits::eHostCached)
                    return true;
                break;
            case VulkanMemoryFeature::HostCoherent:
                if (memType.propertyFlags & vk::MemoryPropertyFlagBits::eHostCoherent)
                    return true;
                break;
            case VulkanMemoryFeature::LazilyAllocated:
                if (memType.propertyFlags & vk::MemoryPropertyFlagBits::eLazilyAllocated)
                    return true;
                break;
            case VulkanMemoryFeature::ProtectedMemory:
                if (memType.propertyFlags & vk::MemoryPropertyFlagBits::eProtected)
                    return true;
                break;
            default:
                break;
        }
    }
    return false;
}

bool VulkanDeviceCapabilities::IsSupported(VulkanShaderFeature feature) const {
    switch (feature) {
        case VulkanShaderFeature::GeometryShader:
            return features.geometryShader;
        case VulkanShaderFeature::TessellationShader:
            return features.tessellationShader;
        case VulkanShaderFeature::ComputeShader:
            return computeQueueFamily.has_value();
        case VulkanShaderFeature::ShaderFloat64:
            return features.shaderFloat64;
        case VulkanShaderFeature::ShaderInt64:
            return features.shaderInt64;
        case VulkanShaderFeature::ShaderInt16:
            return features.shaderInt16;
        case VulkanShaderFeature::ShaderResourceResidency:
            return features.shaderResourceResidency;
        case VulkanShaderFeature::ShaderResourceMinLod:
            return features.shaderResourceMinLod;
        case VulkanShaderFeature::ShaderStorageImageExtendedFormats:
            return features.shaderStorageImageExtendedFormats;
        case VulkanShaderFeature::ShaderStorageImageMultisample:
            return features.shaderStorageImageMultisample;
        case VulkanShaderFeature::ShaderStorageImageReadWithoutFormat:
            return features.shaderStorageImageReadWithoutFormat;
        case VulkanShaderFeature::ShaderStorageImageWriteWithoutFormat:
            return features.shaderStorageImageWriteWithoutFormat;
        case VulkanShaderFeature::ShaderUniformBufferArrayDynamicIndexing:
            return features.shaderUniformBufferArrayDynamicIndexing;
        case VulkanShaderFeature::ShaderSampledImageArrayDynamicIndexing:
            return features.shaderSampledImageArrayDynamicIndexing;
        case VulkanShaderFeature::ShaderStorageBufferArrayDynamicIndexing:
            return features.shaderStorageBufferArrayDynamicIndexing;
        case VulkanShaderFeature::ShaderStorageImageArrayDynamicIndexing:
            return features.shaderStorageImageArrayDynamicIndexing;
        case VulkanShaderFeature::ShaderClipDistance:
            return features.shaderClipDistance;
        case VulkanShaderFeature::ShaderCullDistance:
            return features.shaderCullDistance;
        case VulkanShaderFeature::ShaderDrawParameters:
            return vulkan11Features.shaderDrawParameters;
        default:
            return false;
    }
}

bool VulkanDeviceCapabilities::IsSupported(VulkanRenderFeature feature) const {
    switch (feature) {
        case VulkanRenderFeature::MultiViewport:
            return features.multiViewport;
        case VulkanRenderFeature::SamplerAnisotropy:
            return features.samplerAnisotropy;
        case VulkanRenderFeature::DepthClamp:
            return features.depthClamp;
        case VulkanRenderFeature::DepthBiasClamp:
            return features.depthBiasClamp;
        case VulkanRenderFeature::FillModeNonSolid:
            return features.fillModeNonSolid;
        case VulkanRenderFeature::WideLines:
            return features.wideLines;
        case VulkanRenderFeature::LargePoints:
            return features.largePoints;
        case VulkanRenderFeature::AlphaToOne:
            return features.alphaToOne;
        case VulkanRenderFeature::MultiDrawIndirect:
            return features.multiDrawIndirect;
        case VulkanRenderFeature::DrawIndirectFirstInstance:
            return features.drawIndirectFirstInstance;
        case VulkanRenderFeature::DepthBounds:
            return features.depthBounds;
        case VulkanRenderFeature::LogicOp:
            return features.logicOp;
        case VulkanRenderFeature::SampleRateShading:
            return features.sampleRateShading;
        case VulkanRenderFeature::DualSrcBlend:
            return features.dualSrcBlend;
        case VulkanRenderFeature::MultiView:
            return vulkan11Features.multiview;
        case VulkanRenderFeature::InheritedQueries:
            return features.inheritedQueries;
        case VulkanRenderFeature::OcclusionQueryPrecise:
            return features.occlusionQueryPrecise;
        case VulkanRenderFeature::PipelineStatisticsQuery:
            return features.pipelineStatisticsQuery;
        case VulkanRenderFeature::VertexPipelineStoresAndAtomics:
            return features.vertexPipelineStoresAndAtomics;
        case VulkanRenderFeature::FragmentStoresAndAtomics:
            return features.fragmentStoresAndAtomics;
        default:
            return false;
    }
}

int VulkanDeviceCapabilities::GetDeviceScore() const {
    int score = 0;
    
    // 设备类型得分
    switch (properties.deviceType) {
        case vk::PhysicalDeviceType::eDiscreteGpu:
            score += 1000;
            break;
        case vk::PhysicalDeviceType::eIntegratedGpu:
            score += 500;
            break;
        case vk::PhysicalDeviceType::eVirtualGpu:
            score += 100;
            break;
        case vk::PhysicalDeviceType::eCpu:
            score += 10;
            break;
        default:
            break;
    }
    
    // API版本得分
    score += VK_VERSION_MAJOR(properties.apiVersion) * 10;
    score += VK_VERSION_MINOR(properties.apiVersion);
    
    // 内存得分（每GB +1分）
    vk::DeviceSize totalMemory = 0;
    for (uint32_t i = 0; i < memoryProperties.memoryHeapCount; ++i) {
        if (memoryProperties.memoryHeaps[i].flags & vk::MemoryHeapFlagBits::eDeviceLocal) {
            totalMemory += memoryProperties.memoryHeaps[i].size;
        }
    }
    score += static_cast<int>(totalMemory / (1024ULL * 1024ULL * 1024ULL));
    
    // 特性得分
    if (features.geometryShader) score += 10;
    if (features.tessellationShader) score += 10;
    if (features.samplerAnisotropy) score += 5;
    
    // 扩展得分
    if (IsSupported(VulkanExtensionFeature::RayTracing)) score += 50;
    if (IsSupported(VulkanExtensionFeature::MeshShader)) score += 30;
    if (IsSupported(VulkanExtensionFeature::VariableRateShading)) score += 20;
    
    return score;
}

std::vector<const char*> VulkanDeviceCapabilities::GetRecommendedExtensions() const {
    std::vector<const char*> extensions;
    
    // 必需扩展
    if (CheckExtensionSupport(VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
        extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }
    
    // 推荐扩展
    if (IsSupported(VulkanExtensionFeature::DynamicRendering)) {
        extensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
    }
    
    if (IsSupported(VulkanExtensionFeature::Synchronization2)) {
        extensions.push_back(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);
    }
    
    if (IsSupported(VulkanExtensionFeature::RayTracing)) {
        extensions.push_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
        extensions.push_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
        extensions.push_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
    }
    
    if (CheckExtensionSupport(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME)) {
        extensions.push_back(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
    }
    
    return extensions;
}

void VulkanDeviceCapabilities::PrintCapabilitiesSummary() const {
    std::cout << "\n=== Device Capabilities Summary ===" << std::endl;
    std::cout << "Device: " << properties.deviceName << std::endl;
    std::cout << "Type: ";
    switch (properties.deviceType) {
        case vk::PhysicalDeviceType::eDiscreteGpu:
            std::cout << "Discrete GPU" << std::endl;
            break;
        case vk::PhysicalDeviceType::eIntegratedGpu:
            std::cout << "Integrated GPU" << std::endl;
            break;
        default:
            std::cout << "Other" << std::endl;
    }
    std::cout << "Vulkan API: " << VK_VERSION_MAJOR(properties.apiVersion) << "."
              << VK_VERSION_MINOR(properties.apiVersion) << "."
              << VK_VERSION_PATCH(properties.apiVersion) << std::endl;
    std::cout << "Score: " << GetDeviceScore() << std::endl;
    
    std::cout << "\nSupported Core Features:" << std::endl;
    if (IsSupported(VulkanCoreFeature::Graphics)) std::cout << "  ✓ Graphics" << std::endl;
    if (IsSupported(VulkanCoreFeature::Compute)) std::cout << "  ✓ Compute" << std::endl;
    if (IsSupported(VulkanCoreFeature::Transfer)) std::cout << "  ✓ Transfer" << std::endl;
    
    std::cout << "\nSupported Advanced Features:" << std::endl;
    if (IsSupported(VulkanExtensionFeature::RayTracing)) 
        std::cout << "  ✓ Ray Tracing" << std::endl;
    if (IsSupported(VulkanExtensionFeature::MeshShader)) 
        std::cout << "  ✓ Mesh Shaders" << std::endl;
    if (IsSupported(VulkanExtensionFeature::DynamicRendering)) 
        std::cout << "  ✓ Dynamic Rendering" << std::endl;
}

// VulkanDeviceSelector 实现
VulkanDeviceSelector& VulkanDeviceSelector::RequireCore(VulkanCoreFeature feature) {
    requiredCoreFeatures.push_back(feature);
    return *this;
}

VulkanDeviceSelector& VulkanDeviceSelector::RequireExtension(VulkanExtensionFeature feature) {
    requiredExtensionFeatures.push_back(feature);
    return *this;
}

VulkanDeviceSelector& VulkanDeviceSelector::PreferExtension(VulkanExtensionFeature feature) {
    preferredExtensionFeatures.push_back(feature);
    return *this;
}

bool VulkanDeviceSelector::MeetsRequirements(const VulkanDeviceCapabilities& caps) const {
    // 检查所有必需特性
    for (auto feature : requiredCoreFeatures) {
        if (!caps.IsSupported(feature)) return false;
    }
    for (auto feature : requiredExtensionFeatures) {
        if (!caps.IsSupported(feature)) return false;
    }
    
    // 检查最低API版本
    VulkanAPI deviceVersion(
        VK_VERSION_MAJOR(caps.GetProperties().apiVersion),
        VK_VERSION_MINOR(caps.GetProperties().apiVersion),
        VK_VERSION_PATCH(caps.GetProperties().apiVersion)
    );
    if (!(deviceVersion >= minimumVersion)) return false;
    
    return true;
}

int VulkanDeviceSelector::ScoreDevice(const VulkanDeviceCapabilities& caps) const {
    int score = caps.GetDeviceScore();
    
    // 可选特性加分
    for (auto feature : preferredExtensionFeatures) {
        if (caps.IsSupported(feature)) score += 20;
    }
    
    return score;
}

std::optional<vk::PhysicalDevice> VulkanDeviceSelector::SelectBestDevice(
    const std::vector<vk::PhysicalDevice>& devices) const {
    
    vk::PhysicalDevice bestDevice = nullptr;
    int bestScore = -1;
    VulkanDeviceCapabilities bestCaps;
    
    for (const auto& device : devices) {
        VulkanDeviceCapabilities caps;
        caps.QueryFromDevice(device);
        
        // 检查是否满足必需条件
        if (!MeetsRequirements(caps)) continue;
        
        int score = ScoreDevice(caps);
        if (score > bestScore) {
            bestScore = score;
            bestDevice = device;
            bestCaps = caps;
        }
    }
    
    if (bestDevice) {
        selectedCapabilities = bestCaps;
        return bestDevice;
    }
    
    return std::nullopt;
}

std::optional<VulkanDeviceCapabilities> VulkanDeviceSelector::GetSelectedCapabilities() const {
    return selectedCapabilities;
}

VulkanDeviceSelector& VulkanDeviceSelector::RequireMemory(VulkanMemoryFeature feature) {
    requiredMemoryFeatures.push_back(feature);
    return *this;
}

VulkanDeviceSelector& VulkanDeviceSelector::RequireShader(VulkanShaderFeature feature) {
    requiredShaderFeatures.push_back(feature);
    return *this;
}

VulkanDeviceSelector& VulkanDeviceSelector::RequireRender(VulkanRenderFeature feature) {
    requiredRenderFeatures.push_back(feature);
    return *this;
}

VulkanDeviceSelector& VulkanDeviceSelector::PreferCore(VulkanCoreFeature feature) {
    preferredCoreFeatures.push_back(feature);
    return *this;
}

VulkanDeviceSelector& VulkanDeviceSelector::PreferShader(VulkanShaderFeature feature) {
    preferredShaderFeatures.push_back(feature);
    return *this;
}

VulkanDeviceSelector& VulkanDeviceSelector::PreferRender(VulkanRenderFeature feature) {
    preferredRenderFeatures.push_back(feature);
    return *this;
}

VulkanDeviceSelector& VulkanDeviceSelector::RequireVersion(VulkanAPI version) {
    minimumVersion = version;
    return *this;
}

VulkanDeviceSelector& VulkanDeviceSelector::PreferDiscreteGPU(int priority) {
    discreteGPUPriority = priority;
    return *this;
}

VulkanDeviceSelector& VulkanDeviceSelector::PreferIntegratedGPU(int priority) {
    integratedGPUPriority = priority;
    return *this;
}

// VulkanDeviceCapabilities 补全函数
FeatureSupport VulkanDeviceCapabilities::GetFeatureInfo(VulkanCoreFeature feature) const {
    bool supported = IsSupported(feature);
    std::string name, description;
    
    switch (feature) {
        case VulkanCoreFeature::Graphics:
            name = "Graphics Queue";
            description = "Support for graphics rendering operations";
            break;
        case VulkanCoreFeature::Compute:
            name = "Compute Queue";
            description = "Support for compute shader operations";
            break;
        case VulkanCoreFeature::Transfer:
            name = "Transfer Queue";
            description = "Support for memory transfer operations";
            break;
        case VulkanCoreFeature::SparseBinding:
            name = "Sparse Binding";
            description = "Support for sparse resource binding";
            break;
        case VulkanCoreFeature::Protected:
            name = "Protected Memory";
            description = "Support for protected memory operations (Vulkan 1.1+)";
            break;
    }
    
    return CreateFeatureSupport(supported, name, description);
}

FeatureSupport VulkanDeviceCapabilities::GetFeatureInfo(VulkanMemoryFeature feature) const {
    bool supported = IsSupported(feature);
    std::string name, description;
    
    switch (feature) {
        case VulkanMemoryFeature::DeviceLocal:
            name = "Device Local Memory";
            description = "GPU-local memory for high performance";
            break;
        case VulkanMemoryFeature::HostVisible:
            name = "Host Visible Memory";
            description = "Memory accessible by CPU";
            break;
        case VulkanMemoryFeature::HostCached:
            name = "Host Cached Memory";
            description = "CPU-cached memory for faster reads";
            break;
        case VulkanMemoryFeature::HostCoherent:
            name = "Host Coherent Memory";
            description = "Automatically synchronized between CPU and GPU";
            break;
        case VulkanMemoryFeature::LazilyAllocated:
            name = "Lazily Allocated Memory";
            description = "Memory allocated on first use";
            break;
        case VulkanMemoryFeature::ProtectedMemory:
            name = "Protected Memory";
            description = "DRM-protected memory";
            break;
        default:
            name = "Unknown";
            description = "Unknown memory feature";
            break;
    }
    
    return CreateFeatureSupport(supported, name, description);
}

FeatureSupport VulkanDeviceCapabilities::GetFeatureInfo(VulkanExtensionFeature feature) const {
    bool supported = IsSupported(feature);
    std::string name, description;
    VulkanAPI requiredVer(1, 0, 0);
    
    switch (feature) {
        case VulkanExtensionFeature::Swapchain:
            name = "Swapchain";
            description = "Present rendered images to screen";
            break;
        case VulkanExtensionFeature::RayTracing:
            name = "Ray Tracing";
            description = "Hardware-accelerated ray tracing";
            requiredVer = VulkanAPI(1, 2, 0);
            break;
        case VulkanExtensionFeature::MeshShader:
            name = "Mesh Shaders";
            description = "Advanced geometry processing";
            break;
        case VulkanExtensionFeature::DynamicRendering:
            name = "Dynamic Rendering";
            description = "Renderpass-less rendering (Vulkan 1.3)";
            requiredVer = VulkanAPI(1, 3, 0);
            break;
        default:
            name = "Unknown Extension";
            description = "Unknown extension feature";
            break;
    }
    
    return CreateFeatureSupport(supported, name, description, requiredVer);
}

FeatureSupport VulkanDeviceCapabilities::GetFeatureInfo(VulkanShaderFeature feature) const {
    bool supported = IsSupported(feature);
    std::string name, description;
    
    switch (feature) {
        case VulkanShaderFeature::GeometryShader:
            name = "Geometry Shader";
            description = "Geometry shader stage support";
            break;
        case VulkanShaderFeature::TessellationShader:
            name = "Tessellation Shaders";
            description = "Tessellation control and evaluation shaders";
            break;
        case VulkanShaderFeature::ComputeShader:
            name = "Compute Shader";
            description = "Compute shader support";
            break;
        case VulkanShaderFeature::ShaderFloat64:
            name = "64-bit Float Shaders";
            description = "Double precision floating point in shaders";
            break;
        case VulkanShaderFeature::ShaderInt64:
            name = "64-bit Int Shaders";
            description = "64-bit integer operations in shaders";
            break;
        default:
            name = "Unknown Shader Feature";
            description = "Unknown shader feature";
            break;
    }
    
    return CreateFeatureSupport(supported, name, description);
}

FeatureSupport VulkanDeviceCapabilities::GetFeatureInfo(VulkanRenderFeature feature) const {
    bool supported = IsSupported(feature);
    std::string name, description;
    
    switch (feature) {
        case VulkanRenderFeature::MultiViewport:
            name = "Multiple Viewports";
            description = "Render to multiple viewports simultaneously";
            break;
        case VulkanRenderFeature::SamplerAnisotropy:
            name = "Anisotropic Filtering";
            description = "High-quality texture filtering";
            break;
        case VulkanRenderFeature::DepthClamp:
            name = "Depth Clamp";
            description = "Clamp depth values instead of clipping";
            break;
        case VulkanRenderFeature::WideLines:
            name = "Wide Lines";
            description = "Line width > 1.0";
            break;
        default:
            name = "Unknown Render Feature";
            description = "Unknown render feature";
            break;
    }
    
    return CreateFeatureSupport(supported, name, description);
}

std::vector<VulkanCoreFeature> VulkanDeviceCapabilities::GetSupportedCoreFeatures() const {
    std::vector<VulkanCoreFeature> supported;
    std::vector<VulkanCoreFeature> all = {
        VulkanCoreFeature::Graphics,
        VulkanCoreFeature::Compute,
        VulkanCoreFeature::Transfer,
        VulkanCoreFeature::SparseBinding,
        VulkanCoreFeature::Protected
    };
    
    for (auto feature : all) {
        if (IsSupported(feature)) {
            supported.push_back(feature);
        }
    }
    return supported;
}

std::vector<VulkanMemoryFeature> VulkanDeviceCapabilities::GetSupportedMemoryFeatures() const {
    std::vector<VulkanMemoryFeature> supported;
    std::vector<VulkanMemoryFeature> all = {
        VulkanMemoryFeature::DeviceLocal,
        VulkanMemoryFeature::HostVisible,
        VulkanMemoryFeature::HostCached,
        VulkanMemoryFeature::HostCoherent,
        VulkanMemoryFeature::LazilyAllocated,
        VulkanMemoryFeature::ProtectedMemory
    };
    
    for (auto feature : all) {
        if (IsSupported(feature)) {
            supported.push_back(feature);
        }
    }
    return supported;
}

std::vector<VulkanExtensionFeature> VulkanDeviceCapabilities::GetSupportedExtensionFeatures() const {
    std::vector<VulkanExtensionFeature> supported;
    std::vector<VulkanExtensionFeature> all = {
        VulkanExtensionFeature::Swapchain,
        VulkanExtensionFeature::RayTracing,
        VulkanExtensionFeature::MeshShader,
        VulkanExtensionFeature::DynamicRendering,
        VulkanExtensionFeature::Synchronization2,
        VulkanExtensionFeature::TimelineSemaphore,
        VulkanExtensionFeature::DebugUtils,
        VulkanExtensionFeature::MemoryBudget,
        VulkanExtensionFeature::BufferDeviceAddress,
        VulkanExtensionFeature::DescriptorIndexing
    };
    
    for (auto feature : all) {
        if (IsSupported(feature)) {
            supported.push_back(feature);
        }
    }
    return supported;
}

std::vector<VulkanShaderFeature> VulkanDeviceCapabilities::GetSupportedShaderFeatures() const {
    std::vector<VulkanShaderFeature> supported;
    std::vector<VulkanShaderFeature> all = {
        VulkanShaderFeature::GeometryShader,
        VulkanShaderFeature::TessellationShader,
        VulkanShaderFeature::ComputeShader,
        VulkanShaderFeature::ShaderFloat64,
        VulkanShaderFeature::ShaderInt64,
        VulkanShaderFeature::ShaderInt16
    };
    
    for (auto feature : all) {
        if (IsSupported(feature)) {
            supported.push_back(feature);
        }
    }
    return supported;
}

std::vector<VulkanRenderFeature> VulkanDeviceCapabilities::GetSupportedRenderFeatures() const {
    std::vector<VulkanRenderFeature> supported;
    std::vector<VulkanRenderFeature> all = {
        VulkanRenderFeature::MultiViewport,
        VulkanRenderFeature::SamplerAnisotropy,
        VulkanRenderFeature::DepthClamp,
        VulkanRenderFeature::WideLines,
        VulkanRenderFeature::SampleRateShading,
        VulkanRenderFeature::MultiView
    };
    
    for (auto feature : all) {
        if (IsSupported(feature)) {
            supported.push_back(feature);
        }
    }
    return supported;
}

std::vector<const char*> VulkanDeviceCapabilities::GetRecommendedLayers() const {
    std::vector<const char*> layers;
    
#ifdef _DEBUG
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif
    
    return layers;
}

FeatureSupport VulkanDeviceCapabilities::CreateFeatureSupport(
    bool supported, 
    const std::string& name, 
    const std::string& description,
    VulkanAPI requiredVersion,
    std::vector<std::string> extensions) const {
    
    FeatureSupport fs;
    fs.supported = supported;
    fs.name = name;
    fs.description = description;
    fs.requiredVersion = requiredVersion;
    fs.requiredExtensions = extensions;
    
    return fs;
}