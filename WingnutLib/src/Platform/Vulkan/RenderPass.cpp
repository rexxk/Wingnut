#include "wingnut_pch.h"
#include "RenderPass.h"



namespace Wingnut
{
	
	namespace Vulkan
	{

		Ref<RenderPass> RenderPass::Create(Ref<Device> device, VkFormat format)
		{
			return CreateRef<RenderPass>(device, format);
		}


		RenderPass::RenderPass(Ref<Device> device, VkFormat format)
			: m_Device(device->GetDevice())
		{
			CreateRenderPass(format);
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

		void RenderPass::CreateRenderPass(VkFormat format)
		{
			VkAttachmentDescription colorAttachmentDescription = {};
			colorAttachmentDescription.format = format;
			colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;

			colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			VkAttachmentReference colorAttachmentReference = {};
			colorAttachmentReference.attachment = 0;
			colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkAttachmentDescription depthAttachmentDescription = {};
			depthAttachmentDescription.format = VK_FORMAT_D32_SFLOAT;
			depthAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;

			depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkAttachmentReference depthAttachmentReference = {};
			depthAttachmentReference.attachment = 1;
			depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			std::vector<VkAttachmentDescription> attachments = { colorAttachmentDescription, depthAttachmentDescription };

			VkSubpassDescription subpassDescription = {};
			subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescription.colorAttachmentCount = 1;
			subpassDescription.pColorAttachments = &colorAttachmentReference;
			subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;

			VkSubpassDependency dependency = {};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.srcAccessMask = 0;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			VkRenderPassCreateInfo passCreateInfo = {};
			passCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

			passCreateInfo.attachmentCount = (uint32_t)attachments.size();;
			passCreateInfo.pAttachments = attachments.data();
			passCreateInfo.subpassCount = 1;
			passCreateInfo.pSubpasses = &subpassDescription;

			passCreateInfo.dependencyCount = 1;
			passCreateInfo.pDependencies = &dependency;

			vkCreateRenderPass(m_Device, &passCreateInfo, nullptr, &m_RenderPass);

		}

	}
}
