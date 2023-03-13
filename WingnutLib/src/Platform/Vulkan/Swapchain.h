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
			static Ref<Swapchain> Create(Ref<Device> device, void* surface, VkExtent2D extent);

			Swapchain(Ref<Device> device, void* surface, VkExtent2D extent);
			~Swapchain();

			void Release();

			void Resize(VkSurfaceKHR surface, VkExtent2D newExtent);

			VkExtent2D GetExtent() { return m_Extent; }

			VkSwapchainKHR GetSwapchain() { return m_Swapchain; }

			std::vector<VkImageView>& GetImageViews() { return m_SwapchainImageViews; }
			VkImage GetImage(uint32_t currentFrame) { return m_SwapchainImages[currentFrame]; }

		private:
			VkSwapchainKHR m_Swapchain = nullptr;

			Ref<Device> m_Device = nullptr;

			VkExtent2D m_Extent;

			std::vector<VkImage> m_SwapchainImages;
			std::vector<VkImageView> m_SwapchainImageViews;
		};

	}
}
