#pragma once

#include "Device.h"
#include "RenderPass.h"
#include "Swapchain.h"

#include <vulkan/vulkan.h>


namespace Wingnut
{

	namespace Vulkan
	{

		class Framebuffer
		{
		public:
			Framebuffer(Ref<Device> device, Ref<Swapchain> swapchain, Ref<RenderPass> renderPass, VkImageView depthStencilImageView, VkExtent2D imageExtent);
			~Framebuffer();

			void Release();

			VkFramebuffer GetNextFramebuffer();

		private:
			std::vector<VkFramebuffer> m_Framebuffers;

			VkDevice m_Device = nullptr;

			uint32_t m_ActiveFramebuffer = 0;
		};

	}
}
