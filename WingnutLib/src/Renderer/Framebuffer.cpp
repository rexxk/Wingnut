#include "wingnut_pch.h"
#include "Framebuffer.h"



namespace Wingnut
{


	Framebuffer::Framebuffer(Ref<Device> device, Ref<Swapchain> swapchain, VkExtent2D imageExtent, VkFormat format)
		: m_Device(device->GetDevice())
	{
		VkAttachmentDescription attachmentDescription = {};
		attachmentDescription.format = format;
		attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;

		attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference attachmentReference = {};
		attachmentReference.attachment = 0;
		attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &attachmentReference;

		VkRenderPassCreateInfo passCreateInfo = {};
		passCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

		passCreateInfo.attachmentCount = 1;
		passCreateInfo.pAttachments = &attachmentDescription;
		passCreateInfo.subpassCount = 1;
		passCreateInfo.pSubpasses = &subpassDescription;

		vkCreateRenderPass(m_Device, &passCreateInfo, nullptr, &m_RenderPass);

		for (auto& imageView : swapchain->GetImageViews())
		{
			VkFramebuffer newFramebuffer;

			VkFramebufferCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			createInfo.layers = 1;

			createInfo.renderPass = m_RenderPass;
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

		if (m_RenderPass != nullptr)
		{
			vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
			m_RenderPass = nullptr;
		}
	}


}
