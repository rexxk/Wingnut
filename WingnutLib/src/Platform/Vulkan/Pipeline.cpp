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

		VkCullModeFlagBits CullModeToVulkanCullMode(CullMode mode)
		{
			switch (mode)
			{
				case CullMode::None: return VK_CULL_MODE_NONE;
				case CullMode::Back: return VK_CULL_MODE_BACK_BIT;
				case CullMode::Front: return VK_CULL_MODE_FRONT_BIT;
				case CullMode::FrontAndBack: return VK_CULL_MODE_FRONT_AND_BACK;
			}

			return VK_CULL_MODE_NONE;
		}

		VkFrontFace CullingDirectionToVulkanFrontFace(CullingDirection direction)
		{
			switch (direction)
			{
				case CullingDirection::Clockwise: return VK_FRONT_FACE_CLOCKWISE;
				case CullingDirection::CounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
			}

			return VK_FRONT_FACE_CLOCKWISE;
		}

		VkCompareOp CompareOperationToVulkanCompareOp(CompareOperation compareOperation)
		{
			switch (compareOperation)
			{
				case CompareOperation::Always: return VK_COMPARE_OP_ALWAYS;
				case CompareOperation::Greater: return VK_COMPARE_OP_GREATER;
				case CompareOperation::GreaterOrEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
				case CompareOperation::Equal: return VK_COMPARE_OP_EQUAL;
				case CompareOperation::Less: return VK_COMPARE_OP_LESS;
				case CompareOperation::LessOrEqual: return VK_COMPARE_OP_LESS_OR_EQUAL;
				case CompareOperation::Never: return VK_COMPARE_OP_NEVER;
				case CompareOperation::NotEqual: return VK_COMPARE_OP_NOT_EQUAL;
			}

			return VK_COMPARE_OP_LESS;
		}

		VkBlendFactor BlendStateToVulkanBlendFactor(BlendState state)
		{
			switch (state)
			{
				case BlendState::One: return VK_BLEND_FACTOR_ONE;
				case BlendState::OneMinusSourceAlpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				case BlendState::SourceAlpha: return VK_BLEND_FACTOR_SRC_ALPHA;
				case BlendState::Zero: return VK_BLEND_FACTOR_ZERO;
			}

			return VK_BLEND_FACTOR_SRC_ALPHA;
		}

		Ref<Pipeline> Pipeline::Create(Ref<Device> device, const PipelineSpecification& pipelineSpecification)
		{
			return CreateRef<Pipeline>(device, pipelineSpecification);
		}


		Pipeline::Pipeline(Ref<Device> device, const PipelineSpecification& specification)
			: m_Device(device->GetDevice()), m_Specification(specification)
		{
			CreatePipeline();
		}

		Pipeline::~Pipeline()
		{
			Release();
		}

		void Pipeline::Release()
		{

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

		void Pipeline::CreatePipeline()
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

			std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_LINE_WIDTH ,VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

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

			VkVertexInputBindingDescription vertexInputBindingDescription = {};
			vertexInputBindingDescription.binding = 0;
			vertexInputBindingDescription.stride = m_Specification.PipelineShader->GetVertexStride();
			vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
			vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
			vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
			vertexInputStateCreateInfo.vertexAttributeDescriptionCount = (uint32_t)m_Specification.PipelineShader->GetAttributeDescriptions().size();
			vertexInputStateCreateInfo.pVertexAttributeDescriptions = m_Specification.PipelineShader->GetAttributeDescriptions().data();

			VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
			rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizationStateCreateInfo.depthClampEnable = false;
			rasterizationStateCreateInfo.rasterizerDiscardEnable = false;
			rasterizationStateCreateInfo.polygonMode = PolygonFillTypeToVulkanPolygonMode(rendererSettings.PolygonFill);
			rasterizationStateCreateInfo.lineWidth = rendererSettings.LineWidth;
			rasterizationStateCreateInfo.cullMode = CullModeToVulkanCullMode(m_Specification.CullMode);
			rasterizationStateCreateInfo.frontFace = CullingDirectionToVulkanFrontFace(m_Specification.CullingDirection);

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
			colorBlendAttachment.blendEnable = m_Specification.BlendEnable;
			colorBlendAttachment.srcColorBlendFactor = BlendStateToVulkanBlendFactor(m_Specification.SourceBlendFactor);
			colorBlendAttachment.dstColorBlendFactor = BlendStateToVulkanBlendFactor(m_Specification.DestinationBlendFactor);
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; // ONE; // _MINUS_SRC_ALPHA;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

			VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {};
			colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlendCreateInfo.logicOpEnable = VK_FALSE;
			colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
			colorBlendCreateInfo.attachmentCount = 1;
			colorBlendCreateInfo.pAttachments = &colorBlendAttachment;
			colorBlendCreateInfo.blendConstants[0] = 0.0f;
			colorBlendCreateInfo.blendConstants[1] = 0.0f;
			colorBlendCreateInfo.blendConstants[2] = 0.0f;
			colorBlendCreateInfo.blendConstants[3] = 0.0f;

			std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

			for (auto& descriptorSet : m_Specification.PipelineShader->GetDescriptorSetLayouts())
			{
				descriptorSetLayouts.emplace_back(descriptorSet.second);
			}
			

			VkPipelineLayoutCreateInfo layoutCreateInfo = {};
			layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			layoutCreateInfo.setLayoutCount = (uint32_t)descriptorSetLayouts.size();
			layoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
			layoutCreateInfo.pushConstantRangeCount = 0;
			layoutCreateInfo.pPushConstantRanges = nullptr;


			if (vkCreatePipelineLayout(m_Device, &layoutCreateInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Renderer] Unable to create pipeline layout");
				return;
			}

			LOG_CORE_TRACE("[Renderer] Pipeline layout created");

			VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {};
			depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencilCreateInfo.depthTestEnable = m_Specification.DepthTestEnable;
			depthStencilCreateInfo.depthWriteEnable = m_Specification.DepthWriteEnable;
			depthStencilCreateInfo.depthCompareOp = CompareOperationToVulkanCompareOp(m_Specification.DepthCompareOp);
			depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
			depthStencilCreateInfo.minDepthBounds = 0.0f;
			depthStencilCreateInfo.maxDepthBounds = 1.0f;
			depthStencilCreateInfo.stencilTestEnable = VK_FALSE;
			depthStencilCreateInfo.front = {};
			depthStencilCreateInfo.back = {};

			VkGraphicsPipelineCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			createInfo.renderPass = m_Specification.RenderPass->GetRenderPass();

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
			createInfo.pDepthStencilState = &depthStencilCreateInfo;
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
