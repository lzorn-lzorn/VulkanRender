
#pragma once
#include <optional>
#include <unordered_map>
#include <vulkan/vulkan.hpp>

enum class VulkanCoreFeature {
    Graphics,       
    Compute,
    Transfer,
    SparseBinding,
    Protected  // Vulkan 1.1+
};

// 内存特性
enum class VulkanMemoryFeature {
    DeviceLocal,
    HostVisible,
    HostCached,
    HostCoherent,
    LazilyAllocated,
    DeviceCoherent,
    DeviceUncached,
    ProtectedMemory
};

// 扩展特性
enum class VulkanExtensionFeature {
    // 核心显示扩展
    Swapchain,
    Surface,
    
    // 渲染扩展
    DynamicRendering,      // Vulkan 1.3
    RayTracing,
    RayTracingPipeline,
    RayQuery,
    AccelerationStructure,
    MeshShader,
    TaskShader,
    VariableRateShading,
    FragmentShadingRate,
    
    // 内存管理扩展
    MemoryBudget,
    DedicatedAllocation,
    BindMemory2,
    
    // 同步扩展
    TimelineSemaphore,     // Vulkan 1.2
    Synchronization2,      // Vulkan 1.3
    
    // 调试扩展
    DebugUtils,
    ValidationFeatures,
    
    // 性能扩展
    DescriptorIndexing,
    BufferDeviceAddress,
    ShaderFloat16,
    ShaderInt8,
    Storage8BitAccess,
    Storage16BitAccess,
    
    // 多GPU扩展
    DeviceGroup,
    
    // 其他高级扩展
    ConditionalRendering,
    TransformFeedback,
    VertexAttributeDivisor,
    ExtendedDynamicState,
    ExtendedDynamicState2,
    ExtendedDynamicState3
};

// 着色器特性
enum class VulkanShaderFeature {
    GeometryShader,
    TessellationShader,
    ComputeShader,
    
    // 高级着色器特性
    ShaderFloat64,
    ShaderInt64,
    ShaderInt16,
    ShaderResourceResidency,
    ShaderResourceMinLod,
    ShaderStorageImageExtendedFormats,
    ShaderStorageImageMultisample,
    ShaderStorageImageReadWithoutFormat,
    ShaderStorageImageWriteWithoutFormat,
    ShaderUniformBufferArrayDynamicIndexing,
    ShaderSampledImageArrayDynamicIndexing,
    ShaderStorageBufferArrayDynamicIndexing,
    ShaderStorageImageArrayDynamicIndexing,
    ShaderClipDistance,
    ShaderCullDistance,
    ShaderDrawParameters
};

// 渲染特性
enum class VulkanRenderFeature {
    MultiViewport,
    SamplerAnisotropy,
    DepthClamp,
    DepthBiasClamp,
    FillModeNonSolid,
    WideLines,
    LargePoints,
    AlphaToOne,
    MultiDrawIndirect,
    DrawIndirectFirstInstance,
    DepthBounds,
    LogicOp,
    SampleRateShading,
    DualSrcBlend,
    MultiView,
    VariableMultisampleRate,
    InheritedQueries,
    OcclusionQueryPrecise,
    PipelineStatisticsQuery,
    VertexPipelineStoresAndAtomics,
    FragmentStoresAndAtomics
};

class VulkanAPI {
public:
    VulkanAPI(uint32_t major = 1, uint32_t minor = 0, uint32_t patch = 0)
        : major(major), minor(minor), patch(patch) {}

    uint32_t Major() const { return major; }
    uint32_t Minor() const { return minor; }
    uint32_t Patch() const { return patch; }

    auto Version() const {
        return VK_MAKE_VERSION(major, minor, patch);
    }

    bool operator>=(const VulkanAPI& other) const {
        if (major != other.major) return major > other.major;
        if (minor != other.minor) return minor > other.minor;
        return patch >= other.patch;
    }
private:
    uint32_t major{1};
    uint32_t minor{0};
    uint32_t patch{0};
};

// 特性支持状态
struct FeatureSupport {
    bool supported = false;
    std::string name;
    std::string description;
    VulkanAPI requiredVersion = VulkanAPI(1, 0, 0);
    std::vector<std::string> requiredExtensions;
    std::optional<std::string> limitations;
    
    operator bool() const { return supported; }
};

// 设备能力配置
class VulkanDeviceCapabilities {
public:
    VulkanDeviceCapabilities() = default;
    
    // 从物理设备查询所有能力
    void QueryFromDevice(vk::PhysicalDevice device);
    
    // 检查特定特性
    bool IsSupported(VulkanCoreFeature feature) const;
    bool IsSupported(VulkanMemoryFeature feature) const;
    bool IsSupported(VulkanExtensionFeature feature) const;
    bool IsSupported(VulkanShaderFeature feature) const;
    bool IsSupported(VulkanRenderFeature feature) const;
    
    // 获取详细支持信息
    FeatureSupport GetFeatureInfo(VulkanCoreFeature feature) const;
    FeatureSupport GetFeatureInfo(VulkanMemoryFeature feature) const;
    FeatureSupport GetFeatureInfo(VulkanExtensionFeature feature) const;
    FeatureSupport GetFeatureInfo(VulkanShaderFeature feature) const;
    FeatureSupport GetFeatureInfo(VulkanRenderFeature feature) const;
    
    // 获取所有支持的特性列表
    std::vector<VulkanCoreFeature> GetSupportedCoreFeatures() const;
    std::vector<VulkanMemoryFeature> GetSupportedMemoryFeatures() const;
    std::vector<VulkanExtensionFeature> GetSupportedExtensionFeatures() const;
    std::vector<VulkanShaderFeature> GetSupportedShaderFeatures() const;
    std::vector<VulkanRenderFeature> GetSupportedRenderFeatures() const;
    
    // 获取推荐的扩展列表（根据设备能力）
    std::vector<const char*> GetRecommendedExtensions() const;
    std::vector<const char*> GetRecommendedLayers() const;
    
    // 设备信息
    const vk::PhysicalDeviceProperties& GetProperties() const { return properties; }
    const vk::PhysicalDeviceFeatures& GetFeatures() const { return features; }
    const vk::PhysicalDeviceMemoryProperties& GetMemoryProperties() const { return memoryProperties; }
    
    // 获取队列族索引
    std::optional<uint32_t> GetGraphicsQueueFamily() const { return graphicsQueueFamily; }
    std::optional<uint32_t> GetComputeQueueFamily() const { return computeQueueFamily; }
    std::optional<uint32_t> GetTransferQueueFamily() const { return transferQueueFamily; }
    std::optional<uint32_t> GetPresentQueueFamily() const { return presentQueueFamily; }
    
    // 获取设备得分（用于选择最佳设备）
    int GetDeviceScore() const;
    
    // 打印设备能力摘要
    void PrintCapabilitiesSummary() const;
    
private:
    // 基础设备信息
    vk::PhysicalDevice physicalDevice;
    vk::PhysicalDeviceProperties properties;
    vk::PhysicalDeviceFeatures features;
    vk::PhysicalDeviceMemoryProperties memoryProperties;
    std::vector<vk::QueueFamilyProperties> queueFamilies;
    std::vector<vk::ExtensionProperties> availableExtensions;
    
    // Vulkan 1.1+ 特性
    vk::PhysicalDeviceVulkan11Features vulkan11Features;
    vk::PhysicalDeviceVulkan12Features vulkan12Features;
    vk::PhysicalDeviceVulkan13Features vulkan13Features;
    
    // 扩展特性
    vk::PhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingFeatures;
    vk::PhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures;
    vk::PhysicalDeviceMeshShaderFeaturesEXT meshShaderFeatures;
    vk::PhysicalDeviceFragmentShadingRateFeaturesKHR fragmentShadingRateFeatures;
    vk::PhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures;
    vk::PhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures;
    vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures;
    vk::PhysicalDeviceSynchronization2Features synchronization2Features;
    
    // 队列族索引
    std::optional<uint32_t> graphicsQueueFamily;
    std::optional<uint32_t> computeQueueFamily;
    std::optional<uint32_t> transferQueueFamily;
    std::optional<uint32_t> presentQueueFamily;
    
    // 特性支持缓存
    mutable std::unordered_map<int, FeatureSupport> featureCache;
    
    // 查询函数
    void QueryBasicProperties();
    void QueryQueueFamilies();
    void QueryExtensions();
    void QueryAdvancedFeatures();
    void FindQueueFamilies();
    
    // 特性检查辅助函数
    bool CheckExtensionSupport(const char* extensionName) const;
    FeatureSupport CreateFeatureSupport(
        bool supported, const std::string& name, 
        const std::string& description,
        VulkanAPI requiredVersion = VulkanAPI(1, 0, 0),
        std::vector<std::string> extensions = {}
    ) const;
};

// 设备选择器 - 根据需求选择最佳设备
class VulkanDeviceSelector {
public:
    VulkanDeviceSelector() = default;
    
    // 设置必需特性
    VulkanDeviceSelector& RequireCore(VulkanCoreFeature feature);
    VulkanDeviceSelector& RequireMemory(VulkanMemoryFeature feature);
    VulkanDeviceSelector& RequireExtension(VulkanExtensionFeature feature);
    VulkanDeviceSelector& RequireShader(VulkanShaderFeature feature);
    VulkanDeviceSelector& RequireRender(VulkanRenderFeature feature);
    
    // 设置可选特性（会提高得分）
    VulkanDeviceSelector& PreferCore(VulkanCoreFeature feature);
    VulkanDeviceSelector& PreferExtension(VulkanExtensionFeature feature);
    VulkanDeviceSelector& PreferShader(VulkanShaderFeature feature);
    VulkanDeviceSelector& PreferRender(VulkanRenderFeature feature);
    
    // 设置最低API版本
    VulkanDeviceSelector& RequireVersion(VulkanAPI version);
    
    // 设置设备类型偏好
    VulkanDeviceSelector& PreferDiscreteGPU(int priority = 1000);
    VulkanDeviceSelector& PreferIntegratedGPU(int priority = 500);
    
    // 从设备列表中选择最佳设备
    std::optional<vk::PhysicalDevice> SelectBestDevice(
        const std::vector<vk::PhysicalDevice>& devices) const;
    
    // 获取选中设备的能力
    std::optional<VulkanDeviceCapabilities> GetSelectedCapabilities() const;
    
private:
    std::vector<VulkanCoreFeature> requiredCoreFeatures;
    std::vector<VulkanMemoryFeature> requiredMemoryFeatures;
    std::vector<VulkanExtensionFeature> requiredExtensionFeatures;
    std::vector<VulkanShaderFeature> requiredShaderFeatures;
    std::vector<VulkanRenderFeature> requiredRenderFeatures;
    
    std::vector<VulkanCoreFeature> preferredCoreFeatures;
    std::vector<VulkanExtensionFeature> preferredExtensionFeatures;
    std::vector<VulkanShaderFeature> preferredShaderFeatures;
    std::vector<VulkanRenderFeature> preferredRenderFeatures;
    
    VulkanAPI minimumVersion = VulkanAPI(1, 0, 0);
    int discreteGPUPriority = 1000;
    int integratedGPUPriority = 500;
    
    mutable std::optional<VulkanDeviceCapabilities> selectedCapabilities;
    
    int ScoreDevice(const VulkanDeviceCapabilities& caps) const;
    bool MeetsRequirements(const VulkanDeviceCapabilities& caps) const;
};