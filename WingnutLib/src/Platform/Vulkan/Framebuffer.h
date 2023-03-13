#pragma once

#include "Device.h"
#include "RenderPass.h"

#include <vulkan/vulkan.h>


namespace Wingnut
{

	namespace Vulkan
	{

		class Framebuffer
		{
		public:
			static Ref<Framebuffer> Create(Ref<Device> device, Ref<RenderPass> renderPass, VkExtent2D imageExtent, const std::vector<VkImageView>& imageViews, VkImageView depthStencilView);

			Framebuffer(Ref<Device> device, Ref<RenderPass> renderPass, VkExtent2D imageExtent, const std::vector<VkImageView>& imageViews, VkImageView depthStencilView);
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
