#include "wingnut_pch.h"
#include "RenderPass.h"



namespace Wingnut
{
	
	namespace Vulkan
	{

		RenderPass::RenderPass(Ref<Device> device, VkFormat format)
			: m_Device(device->GetDevice())
		{
			Create(format);
		}

		RenderPass::~RenderPass()
		{
			Release();
		}

		void RenderPass::Release()
		{
			if (m_RenderPass != nullptr)
			{
				vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
				m_RenderPass = nullptr;
			}
		}

		void RenderPass::Create(VkFormat format)
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

		}

	}
}
