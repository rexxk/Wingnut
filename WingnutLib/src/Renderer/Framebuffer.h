#pragma once

#include "Device.h"
#include "RenderPass.h"
#include "Swapchain.h"

#include <vulkan/vulkan.h>


namespace Wingnut
{


	class Framebuffer
	{
	public:
		Framebuffer(Ref<Device> device, Ref<Swapchain> swapchain, Ref<RenderPass> renderPass, VkExtent2D imageExtent);
		~Framebuffer();

		void Release();

	private:
		std::vector<VkFramebuffer> m_Framebuffers;

		VkDevice m_Device = nullptr;


	};


}
