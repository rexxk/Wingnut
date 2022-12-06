#include "wingnut_pch.h"
#include "VulkanSwapchain.h"

#include "VulkanDevice.h"


namespace Wingnut
{

	VulkanSwapchain::VulkanSwapchain(Ref<RendererDevice> device, void* surface)
		: m_Device(device)
	{

		Reset((VkSurfaceKHR)surface);

	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		Release();
	}

	void VulkanSwapchain::Release()
	{
		if (m_Swapchain != nullptr)
		{
			vkDestroySwapchainKHR((VkDevice)m_Device->GetDevice(), m_Swapchain, nullptr);
			m_Swapchain = nullptr;
		}
	}

	void VulkanSwapchain::Reset(VkSurfaceKHR surface)
	{
		VulkanPhysicalDeviceProperties deviceProperties = *(VulkanPhysicalDeviceProperties*)m_Device->GetDeviceProperties();


		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

		createInfo.oldSwapchain = nullptr;

		createInfo.surface = surface;

		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;

		createInfo.imageArrayLayers = 1;
		createInfo.minImageCount = 2;

		createInfo.imageExtent = deviceProperties.SurfaceCapabilities.currentExtent;
		createInfo.imageFormat = deviceProperties.SurfaceFormat.format;
		createInfo.imageColorSpace = deviceProperties.SurfaceFormat.colorSpace;
		createInfo.preTransform = deviceProperties.SurfaceCapabilities.currentTransform;

		if (vkCreateSwapchainKHR((VkDevice)m_Device->GetDevice(), &createInfo, nullptr, &m_Swapchain) != VK_SUCCESS)
		{
			LOG_CORE_ERROR("[Renderer] Unable to create Vulkan swapchain");
			return;
		}

		uint32_t imageCount = 0;
		vkGetSwapchainImagesKHR((VkDevice)m_Device->GetDevice(), m_Swapchain, &imageCount, nullptr);
		m_SwapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR((VkDevice)m_Device->GetDevice(), m_Swapchain, &imageCount, m_SwapchainImages.data());


		LOG_CORE_TRACE("[Renderer] Created Vulkan swapchain");
	}


}
