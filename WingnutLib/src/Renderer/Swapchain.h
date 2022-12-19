#pragma once

#include "Device.h"
#include "Swapchain.h"

#include <vulkan/vulkan.h>


namespace Wingnut
{


	class Swapchain
	{
	public:
		Swapchain(Ref<Device> device, void* surface, VkExtent2D extent);
		~Swapchain();

		void Release();

		void Resize(VkSurfaceKHR surface, VkExtent2D newExtent);

		VkSwapchainKHR GetSwapchain() { return m_Swapchain; }

		std::vector<VkImageView>& GetImageViews() { return m_SwapchainImageViews; }

	private:
		VkSwapchainKHR m_Swapchain = nullptr;

		Ref<Device> m_Device = nullptr;


		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainImageViews;
	};


}
