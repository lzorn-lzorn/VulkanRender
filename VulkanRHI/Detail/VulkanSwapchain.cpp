#include "../Detail/VulkanSwapchain.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"
#include <algorithm>
#include <cstdint>
#include <limits>


VulkanSwapchain::VulkanSwapchain(vk::Instance vkInstance, VulkanDevice& device, vk::SurfaceKHR surface, size_t width, size_t height)
	: mVkSurface(surface)
	, mDevice(device)
	, mWidth(width)
	, mHeight(height)
{
	mVkSwapchain = CreateSwapChain();
}

vk::SwapchainKHR VulkanSwapchain::CreateSwapChain(){
	vk::SwapchainCreateInfoKHR vkCreateSwapChainInfo;
    vkCreateSwapChainInfo
		.setClipped(true)
		.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
		.setImageExtent(mSwapChainInfo.vkExtent2D)
		.setImageColorSpace(mSwapChainInfo.vkFormat.colorSpace)
		.setImageFormat(mSwapChainInfo.vkFormat.format)
		.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
		.setMinImageCount(mSwapChainInfo.count)
		.setImageArrayLayers(1)
		.setPresentMode(vk::PresentModeKHR::eFifo)
		.setPreTransform(mSwapChainInfo.vkTransform)
		.setSurface(mVkSurface)
		.setImageSharingMode(vk::SharingMode::eExclusive);

    return mDevice.GetVkDevice().createSwapchainKHR(vkCreateSwapChainInfo);
}

std::vector<vk::Image> VulkanSwapchain::GetVkImages(){
	return mDevice.GetVkDevice().getSwapchainImagesKHR(mVkSwapchain);
}

void VulkanSwapchain::InitSwapChainInfo(const size_t width, const size_t height){
	mSwapChainInfo.vkFormat = this->QuerySurfaceFormat();
	vk::SurfaceCapabilitiesKHR capability = mDevice.GetPhysicalDevice().getSurfaceCapabilitiesKHR(mVkSurface);
	mSwapChainInfo.count = std::clamp(capability.minImageCount+1, capability.minImageCount, capability.maxImageCount);
	mSwapChainInfo.vkTransform = capability.currentTransform;
	mSwapChainInfo.vkExtent2D = this->QuerySurfaceExtent(capability, width, height);
}

vk::SurfaceFormatKHR VulkanSwapchain::QuerySurfaceFormat(){
	auto formats = mDevice.GetPhysicalDevice().getSurfaceFormatsKHR(mVkSurface);
	for (const auto& format : formats){
		if (format.format == vk::Format::eR8G8B8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			return format;
		}
	}
	return *formats.begin();
}

vk::Extent2D VulkanSwapchain::QuerySurfaceExtent(const vk::SurfaceCapabilitiesKHR& capability, const size_t width, const size_t height){
	vk::Extent2D ret {0, 0};
	if (capability.currentExtent.width != std::numeric_limits<uint32_t>::max()){
		ret = capability.currentExtent;
	} else {
		ret.setHeight(std::clamp(ret.height, capability.minImageExtent.height, capability.maxImageExtent.height));
		ret.setWidth(std::clamp(ret.width, capability.minImageExtent.width, capability.maxImageExtent.width));
	}
	return ret;
}