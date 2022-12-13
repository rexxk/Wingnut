#pragma once

#include <vulkan/vulkan.h>

#include "Device.h"
#include "Swapchain.h"


namespace Wingnut
{


	class Framebuffer
	{
	public:
		Framebuffer(Ref<Device> device, Ref<Swapchain> swapchain, VkExtent2D imageExtent, VkFormat format);
		~Framebuffer();

		void Release();

	private:
		std::vector<VkFramebuffer> m_Framebuffers;

		VkRenderPass m_RenderPass = nullptr;

		VkDevice m_Device = nullptr;


	};


}
