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
			static Ref<Framebuffer> Create(Ref<Device> device, Ref<Swapchain> swapchain, Ref<RenderPass> renderPass, VkExtent2D imageExtent, VkImageView depthStencilImageView, VkImageView renderToTexture = nullptr);

			Framebuffer(Ref<Device> device, Ref<Swapchain> swapchain, Ref<RenderPass> renderPass, VkExtent2D imageExtent, VkImageView depthStencilImageView, VkImageView renderToTexture);
			~Framebuffer();

			void Release();

			VkFramebuffer GetFramebuffer() { return m_Framebuffers[m_ActiveFramebuffer]; }
			VkFramebuffer GetNextFramebuffer();

		private:
			std::vector<VkFramebuffer> m_Framebuffers;

			VkDevice m_Device = nullptr;

			uint32_t m_ActiveFramebuffer = 0;
		};

	}
}
