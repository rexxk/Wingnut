#pragma once

#include "Device.h"
#include "Swapchain.h"

#include <vulkan/vulkan.h>


namespace Wingnut
{


	class Swapchain
	{
	public:
		Swapchain(Ref<Device> device, void* surface);
		~Swapchain();

		void Release();

	private:
		void Reset(VkSurfaceKHR surface);

	private:
		VkSwapchainKHR m_Swapchain = nullptr;

		Ref<Device> m_Device = nullptr;


		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainImageViews;
	};


}
