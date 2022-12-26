#include "wingnut_pch.h"
#include "Pipeline.h"

#include "Renderer/Renderer.h"


namespace Wingnut
{

	namespace Vulkan
	{

		VkPolygonMode PolygonFillTypeToVulkanPolygonMode(PolygonFillType fillType)
		{
			switch (fillType)
			{
				case PolygonFillType::Point: return VK_POLYGON_MODE_POINT;
				case PolygonFillType::Wireframe: return VK_POLYGON_MODE_LINE;
				case PolygonFillType::Solid: return VK_POLYGON_MODE_FILL;
			}

			return VK_POLYGON_MODE_FILL;
		}


		VkShaderStageFlagBits ShaderDomainToShaderStageBit(ShaderDomain domain)
		{
			switch (domain)
			{
				case ShaderDomain::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
				case ShaderDomain::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
				case ShaderDomain::Compute: return VK_SHADER_STAGE_COMPUTE_BIT;
			}

			return VK_SHADER_STAGE_VERTEX_BIT;
		}



		Pipeline::Pipeline(Ref<Device> device, Ref<RenderPass> renderPass, const PipelineSpecification& specification)
			: m_Device(device->GetDevice()), m_Specification(specification)
		{
			Create(renderPass);
		}

		Pipeline::~Pipeline()
		{
			Release();
		}

		void Pipeline::Release()
		{
			if (m_Specification.PipelineShader != nullptr)
			{
				m_Specification.PipelineShader->Release();
			}

			if (m_PipelineLayout != nullptr)
			{
				vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
				m_PipelineLayout = nullptr;
			}

			if (m_Pipeline != nullptr)
			{
				vkDestroyPipeline(m_Device, m_Pipeline, nullptr);
				m_Pipeline = nullptr;
			}

			if (m_PipelineCache != nullptr)
			{
				vkDestroyPipelineCache(m_Device, m_PipelineCache, nullptr);
				m_PipelineCache = nullptr;
			}

		}

		void Pipeline::Create(Ref<RenderPass> renderPass)
		{
			RendererSettings& rendererSettings = Renderer::GetRendererSettings();

			VkViewport viewport = {};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)m_Specification.Extent.width;
			viewport.height = (float)m_Specification.Extent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			VkRect2D scissor = {};
			scissor.offset = { 0, 0 };
			scissor.extent = m_Specification.Extent;

			std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_LINE_WIDTH /*, VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR */ };

			VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
			dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicStateCreateInfo.dynamicStateCount = (uint32_t)dynamicStates.size();
			dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

			VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
			viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportStateCreateInfo.viewportCount = 1;
			viewportStateCreateInfo.pViewports = &viewport;
			viewportStateCreateInfo.scissorCount = 1;
			viewportStateCreateInfo.pScissors = &scissor;

			VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {};
			inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssemblyCreateInfo.primitiveRestartEnable = false;

			std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

			for (auto& shaderModule : m_Specification.PipelineShader->GetShaderModules())
			{
				VkPipelineShaderStageCreateInfo stageCreateInfo = {};
				stageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				stageCreateInfo.stage = ShaderDomainToShaderStageBit(shaderModule.Domain);
				stageCreateInfo.pName = "main";
				stageCreateInfo.module = shaderModule.Module;

				shaderStages.emplace_back(stageCreateInfo);
			}

			std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

			{
				VkVertexInputAttributeDescription attribDescription;
				attribDescription.binding = 0;
				attribDescription.location = 0;
				attribDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
				attribDescription.offset = 0;

				attributeDescriptions.emplace_back(attribDescription);
			}
			{
				VkVertexInputAttributeDescription attribDescription;
				attribDescription.binding = 0;
				attribDescription.location = 1;
				attribDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
				attribDescription.offset = 12;

				attributeDescriptions.emplace_back(attribDescription);
			}

			VkVertexInputBindingDescription vertexInputBindingDescription = {};
			vertexInputBindingDescription.binding = 0;
			vertexInputBindingDescription.stride = 28;
			vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
			vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
			vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
			vertexInputStateCreateInfo.vertexAttributeDescriptionCount = (uint32_t)attributeDescriptions.size();
			vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

			VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
			rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizationStateCreateInfo.depthClampEnable = false;
			rasterizationStateCreateInfo.rasterizerDiscardEnable = false;
			rasterizationStateCreateInfo.polygonMode = PolygonFillTypeToVulkanPolygonMode(rendererSettings.PolygonFill);
			rasterizationStateCreateInfo.lineWidth = rendererSettings.LineWidth;
			rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
			rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;

			rasterizationStateCreateInfo.depthBiasEnable = false;
			rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
			rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
			rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

			VkPipelineMultisampleStateCreateInfo multisampleCreateInfo = {};
			multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampleCreateInfo.sampleShadingEnable = false;
			multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			multisampleCreateInfo.minSampleShading = 1.0f;
			multisampleCreateInfo.pSampleMask = nullptr;
			multisampleCreateInfo.alphaToCoverageEnable = false;
			multisampleCreateInfo.alphaToOneEnable = false;

			VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachment.blendEnable = true;
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

			VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {};
			colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlendCreateInfo.logicOpEnable = false;
			colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
			colorBlendCreateInfo.attachmentCount = 1;
			colorBlendCreateInfo.pAttachments = &colorBlendAttachment;
			colorBlendCreateInfo.blendConstants[0] = 0.0f;
			colorBlendCreateInfo.blendConstants[1] = 0.0f;
			colorBlendCreateInfo.blendConstants[2] = 0.0f;
			colorBlendCreateInfo.blendConstants[3] = 0.0f;

			VkPipelineLayoutCreateInfo layoutCreateInfo = {};
			layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			layoutCreateInfo.setLayoutCount = 0;
			layoutCreateInfo.pSetLayouts = nullptr;
			layoutCreateInfo.pushConstantRangeCount = 0;
			layoutCreateInfo.pPushConstantRanges = nullptr;


			if (vkCreatePipelineLayout(m_Device, &layoutCreateInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Renderer] Unable to create pipeline layout");
				return;
			}

			LOG_CORE_TRACE("[Renderer] Pipeline layout created");


			VkGraphicsPipelineCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			createInfo.renderPass = renderPass->GetRenderPass();

			createInfo.layout = m_PipelineLayout;
			createInfo.stageCount = (uint32_t)shaderStages.size();
			createInfo.pStages = shaderStages.data();

			createInfo.pRasterizationState = &rasterizationStateCreateInfo;
			createInfo.pDynamicState = &dynamicStateCreateInfo;
			createInfo.pViewportState = &viewportStateCreateInfo;
			createInfo.pVertexInputState = &vertexInputStateCreateInfo;
			createInfo.pMultisampleState = &multisampleCreateInfo;
			createInfo.pColorBlendState = &colorBlendCreateInfo;
			createInfo.pInputAssemblyState = &inputAssemblyCreateInfo;

			createInfo.pVertexInputState = &vertexInputStateCreateInfo;


			VkPipelineCacheCreateInfo cacheCreateInfo = {};
			cacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

			if (vkCreatePipelineCache(m_Device, &cacheCreateInfo, nullptr, &m_PipelineCache) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Renderer] Failed to create pipeline cache");
				return;
			}

			LOG_CORE_TRACE("[Renderer] Created graphics pipeline cache");

			if (vkCreateGraphicsPipelines(m_Device, m_PipelineCache, 1, &createInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Renderer] Failed to create graphics pipeline");
				return;
			}

			LOG_CORE_TRACE("[Renderer] Created graphics pipeline");


		}

	}
}
