#pragma once
#include <vulkan/vulkan.hpp>

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
    uint32_t idxFamily{0};
    uint32_t idxQueue{0};

};