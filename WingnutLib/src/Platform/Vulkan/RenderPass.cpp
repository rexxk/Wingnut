#include "wingnut_pch.h"
#include "RenderPass.h"



namespace Wingnut
{
	
	namespace Vulkan
	{

		VkAttachmentLoadOp AttachmentLoadOpToVulkanAttachmentLoadOp(AttachmentLoadOp loadOp)
		{
			switch (loadOp)
			{
				case AttachmentLoadOp::Clear: return VK_ATTACHMENT_LOAD_OP_CLEAR;
				case AttachmentLoadOp::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				case AttachmentLoadOp::Load: return VK_ATTACHMENT_LOAD_OP_LOAD;
			}

			return VK_ATTACHMENT_LOAD_OP_CLEAR;
		}

		VkAttachmentStoreOp AttachmentStoreOpToVulkanAttachmentStoreOp(AttachmentStoreOp storeOp)
		{
			switch (storeOp)
			{
				case AttachmentStoreOp::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
				case AttachmentStoreOp::None: return VK_ATTACHMENT_STORE_OP_NONE;
				case AttachmentStoreOp::Store: return VK_ATTACHMENT_STORE_OP_STORE;
			}

			return VK_ATTACHMENT_STORE_OP_STORE;
		}

		Ref<RenderPass> RenderPass::Create(Ref<Device> device, const RenderPassSpecification& specification)
		{
			return CreateRef<RenderPass>(device, specification);
		}


		RenderPass::RenderPass(Ref<Device> device, const RenderPassSpecification& specification)
			: m_Device(device->GetDevice()), m_Specification(specification)
		{
			CreateRenderPass();
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

		void RenderPass::CreateRenderPass()
		{
			VkAttachmentDescription colorAttachmentDescription = {};
			colorAttachmentDescription.format = m_Specification.Format;
			colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;

			colorAttachmentDescription.loadOp = AttachmentLoadOpToVulkanAttachmentLoadOp(m_Specification.LoadOp);
			colorAttachmentDescription.storeOp = AttachmentStoreOpToVulkanAttachmentStoreOp(m_Specification.StoreOp);

			colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			if (m_Specification.LoadOp == AttachmentLoadOp::Load)
			{
				colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			}
			else
			{
				colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			}
			colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			VkAttachmentReference colorAttachmentReference = {};
			colorAttachmentReference.attachment = 0;
			colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkAttachmentDescription depthAttachmentDescription = {};
			depthAttachmentDescription.format = VK_FORMAT_D32_SFLOAT;
			depthAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;

			depthAttachmentDescription.loadOp = AttachmentLoadOpToVulkanAttachmentLoadOp(m_Specification.LoadOp);
			depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			if (m_Specification.LoadOp == AttachmentLoadOp::Load)
			{
				depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}
			else
			{
				depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			}

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
