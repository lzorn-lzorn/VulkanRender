#pragma once
#include "VulkanEnv.h"
#include <vulkan/vulkan.hpp>
enum class VulkanQueueType{
    NotValid = 0,
    Graphics = 1,
    Present  = 2,
    Compute  = 3,
    Transfer = 4,
};
class VulkanQueue{
public:
    VulkanQueue(){}
    ~VulkanQueue(){}

    uint32_t GetFamilyIndex() const {
        return idxFamily;
    }

    uint32_t GetQueueIndex() const {
        return idxQueue;
    }
private:
    vk::Queue vkQueue;
    uint32_t idxFamily{0}; // vulkan 对应的队列族索引
    uint32_t idxQueue{0};
    VulkanQueueType type{ VulkanQueueType::NotValid };
};