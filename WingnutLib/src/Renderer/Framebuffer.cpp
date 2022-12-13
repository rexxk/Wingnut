#include "wingnut_pch.h"
#include "Framebuffer.h"



namespace Wingnut
{


	Framebuffer::Framebuffer(Ref<Device> device, Ref<Swapchain> swapchain, Ref<RenderPass> renderPass, VkExtent2D imageExtent)
		: m_Device(device->GetDevice())
	{

		for (auto& imageView : swapchain->GetImageViews())
		{
			VkFramebuffer newFramebuffer;

			VkFramebufferCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			createInfo.layers = 1;

			createInfo.renderPass = renderPass->GetRenderPass();
			createInfo.width = imageExtent.width;
			createInfo.height = imageExtent.height;

			createInfo.attachmentCount = 1;
			createInfo.pAttachments = &imageView;

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


}
