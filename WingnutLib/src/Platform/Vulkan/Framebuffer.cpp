#include "wingnut_pch.h"
#include "Framebuffer.h"



namespace Wingnut
{

	namespace Vulkan
	{


		Ref<Framebuffer> Framebuffer::Create(Ref<Device> device, Ref<RenderPass> renderPass, VkExtent2D imageExtent, const std::vector<VkImageView>& imageViews, VkImageView depthStencilView)
		{
			return CreateRef<Framebuffer>(device, renderPass, imageExtent, imageViews, depthStencilView);
		}


		Framebuffer::Framebuffer(Ref<Device> device, Ref<RenderPass> renderPass, VkExtent2D imageExtent, const std::vector<VkImageView>& imageViews, VkImageView depthStencilView)
			: m_Device(device->GetDevice())
		{

			for (auto& imageView : imageViews)
			{
				VkFramebuffer newFramebuffer;

				VkFramebufferCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				createInfo.layers = 1;

				createInfo.renderPass = renderPass->GetRenderPass();
				createInfo.width = imageExtent.width;
				createInfo.height = imageExtent.height;

				std::vector<VkImageView> framebufferImageViews = { imageView, depthStencilView };

				createInfo.attachmentCount = (uint32_t)framebufferImageViews.size();
				createInfo.pAttachments = framebufferImageViews.data();


				if (vkCreateFramebuffer(m_Device, &createInfo, nullptr, &newFramebuffer) != VK_SUCCESS)
				{
					LOG_CORE_ERROR("[Renderer] Failed to create framebuffer");
					return;
				}

				m_Framebuffers.emplace_back(newFramebuffer);

				LOG_CORE_TRACE("[Renderer] Framebuffer created");
			}

		}

		Framebuffer::~Framebuffer()
		{
			Release();
		}

		void Framebuffer::Release()
		{
			for (auto& framebuffer : m_Framebuffers)
			{
				vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
				framebuffer = nullptr;
			}

			m_Framebuffers.clear();
		}

		VkFramebuffer Framebuffer::GetNextFramebuffer()
		{
			if (m_ActiveFramebuffer >= (uint32_t)m_Framebuffers.size())
			{
				m_ActiveFramebuffer = 0;
			}

			return m_Framebuffers[m_ActiveFramebuffer++];
		}

	}
}
