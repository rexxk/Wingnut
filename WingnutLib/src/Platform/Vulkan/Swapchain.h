#pragma once

#include "Device.h"
#include "Swapchain.h"

#include <vulkan/vulkan.h>


namespace Wingnut
{

	namespace Vulkan
	{

		class Swapchain
		{
		public:
			Swapchain(Ref<Device> device, void* surface, VkExtent2D extent);
			~Swapchain();

			void Release();

			void Resize(VkSurfaceKHR surface, VkExtent2D newExtent);

			VkExtent2D GetExtent() { return m_Extent; }

			VkSwapchainKHR GetSwapchain() { return m_Swapchain; }

			std::vector<VkImageView>& GetImageViews() { return m_SwapchainImageViews; }

		private:
			VkSwapchainKHR m_Swapchain = nullptr;

			Ref<Device> m_Device = nullptr;

			VkExtent2D m_Extent;

			std::vector<VkImage> m_SwapchainImages;
			std::vector<VkImageView> m_SwapchainImageViews;
		};

	}
}
