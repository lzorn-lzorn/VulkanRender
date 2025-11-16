#include "../Detail/VulkanDevice.h"
#include "../Detail/VulkanSupport.h"


 VulkanDevice::VulkanDevice(VulkanEnv*env, vk::PhysicalDevice * physicalDevice)
	: vkEnv(env) , ptrCurPhysicalDevice(physicalDevice) 
{
	auto vkPhysicalDevices = VulkanEnv::Instance().VkInstance().enumeratePhysicalDevices();
	*ptrCurPhysicalDevice = GetBestPhysicalDevice(vkPhysicalDevices);
	CreateDevice();
};
void VulkanDevice::CreateDevice() {

}

VulkanDevice::~VulkanDevice(){
    device.destroy();
    ptrCurPhysicalDevice = nullptr;
}