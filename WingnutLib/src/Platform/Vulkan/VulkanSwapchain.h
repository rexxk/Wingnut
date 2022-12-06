#pragma once

#include "Renderer/RendererDevice.h"
#include "Renderer/RendererSwapchain.h"

#include <vulkan/vulkan.h>


namespace Wingnut
{


	class VulkanSwapchain : public RendererSwapchain
	{
	public:
		VulkanSwapchain(Ref<RendererDevice> device, void* surface);
		~VulkanSwapchain();

		virtual void Release() override;

	private:
		void Reset(VkSurfaceKHR surface);

	private:
		VkSwapchainKHR m_Swapchain = nullptr;

		Ref<RendererDevice> m_Device = nullptr;


		std::vector<VkImage> m_SwapchainImages;
	};


}
