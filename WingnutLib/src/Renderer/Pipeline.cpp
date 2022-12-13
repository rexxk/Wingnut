#include "wingnut_pch.h"
#include "Pipeline.h"




namespace Wingnut
{


	Pipeline::Pipeline(Ref<Device> device, Ref<RenderPass> renderPass)
		: m_Device(device->GetDevice())
	{
		Create(renderPass);
	}

	Pipeline::~Pipeline()
	{
		Release();
	}

	void Pipeline::Release()
	{
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

		VkGraphicsPipelineCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		createInfo.renderPass = renderPass->GetRenderPass();


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
