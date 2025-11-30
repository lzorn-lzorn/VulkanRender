#include "../Detail/VulkanDevice.h"
#include "../Detail/VulkanSupport.h"


 VulkanDevice::VulkanDevice(VulkanEnv* env, vk::PhysicalDevice vkPhysicalDevice)
	: mVkEnv(*env)
	, mVkPhysicalDevice(vkPhysicalDevice)
{

	CreateDevice();
};
void VulkanDevice::CreateDevice() {

}

VulkanDevice::~VulkanDevice(){
    mVkDevice.destroy();
}