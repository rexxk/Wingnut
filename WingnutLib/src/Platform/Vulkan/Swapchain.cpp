#include "wingnut_pch.h"
#include "Swapchain.h"

#include "Device.h"


namespace Wingnut
{
	
	namespace Vulkan
	{

		Swapchain::Swapchain(Ref<Device> device, void* surface, VkExtent2D extent)
			: m_Device(device)
		{

			Resize((VkSurfaceKHR)surface, extent);

		}

		Swapchain::~Swapchain()
		{
			Release();
		}

		void Swapchain::Release()
		{
			for (VkImageView imageView : m_SwapchainImageViews)
			{
				vkDestroyImageView(m_Device->GetDevice(), imageView, nullptr);
			}

			m_SwapchainImageViews.clear();

			if (m_Swapchain != nullptr)
			{
				vkDestroySwapchainKHR(m_Device->GetDevice(), m_Swapchain, nullptr);
				m_Swapchain = nullptr;
			}
		}

		void Swapchain::Resize(VkSurfaceKHR surface, VkExtent2D extent)
		{
			PhysicalDeviceProperties deviceProperties = m_Device->GetDeviceProperties();

			VkSwapchainKHR newSwapchain = nullptr;

			VkSwapchainCreateInfoKHR createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

			createInfo.oldSwapchain = m_Swapchain;

			createInfo.surface = surface;

			createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

			createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			createInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;

			createInfo.imageArrayLayers = 1;
			createInfo.minImageCount = 2;

			createInfo.imageExtent = extent;
			createInfo.imageFormat = deviceProperties.SurfaceFormat.format;
			createInfo.imageColorSpace = deviceProperties.SurfaceFormat.colorSpace;
			createInfo.preTransform = deviceProperties.SurfaceCapabilities.currentTransform;

			if (vkCreateSwapchainKHR((VkDevice)m_Device->GetDevice(), &createInfo, nullptr, &newSwapchain) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Renderer] Unable to create Vulkan swapchain");
				return;
			}

			uint32_t imageCount = 0;
			vkGetSwapchainImagesKHR((VkDevice)m_Device->GetDevice(), newSwapchain, &imageCount, nullptr);
			m_SwapchainImages.resize(imageCount);
			vkGetSwapchainImagesKHR((VkDevice)m_Device->GetDevice(), newSwapchain, &imageCount, m_SwapchainImages.data());


			LOG_CORE_TRACE("[Renderer] Created Vulkan swapchain");

			for (VkImageView imageView : m_SwapchainImageViews)
			{
				vkDestroyImageView(m_Device->GetDevice(), imageView, nullptr);
			}

			m_SwapchainImageViews.clear();

			for (VkImage& image : m_SwapchainImages)
			{
				VkImageView imageView = nullptr;

				VkImageViewCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				createInfo.image = image;
				createInfo.format = deviceProperties.SurfaceFormat.format;
				createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

				createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;

				createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				createInfo.subresourceRange.baseMipLevel = 0;
				createInfo.subresourceRange.levelCount = 1;
				createInfo.subresourceRange.layerCount = 1;
				createInfo.subresourceRange.baseArrayLayer = 0;

				if (vkCreateImageView((VkDevice)m_Device->GetDevice(), &createInfo, nullptr, &imageView) != VK_SUCCESS)
				{
					LOG_CORE_ERROR("[Renderer] Failed to create imageview for swapchain image");
					return;
				}

				m_SwapchainImageViews.emplace_back(imageView);

			}

			m_Swapchain = newSwapchain;
		}

	}
}
