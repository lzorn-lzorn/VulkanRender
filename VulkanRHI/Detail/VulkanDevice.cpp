#include "../Detail/VulkanDevice.h"
#include "../Detail/VulkanSupport.h"


 VulkanDevice::VulkanDevice(VulkanEnv* env, vk::SurfaceKHR surface)
	: vk_env(env)
{
	auto vkPhysicalDevices = VulkanEnv::Instance().VkInstance().enumeratePhysicalDevices();
	CreateDevice();
};
void VulkanDevice::CreateDevice() {

}

VulkanDevice::~VulkanDevice(){
    vk_device.destroy();
}