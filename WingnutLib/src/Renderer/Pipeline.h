#pragma once

#include "Device.h"
#include "RenderPass.h"

#include <vulkan/vulkan.h>



namespace Wingnut
{


	class Pipeline
	{
	public:
		Pipeline(Ref<Device> device, Ref<RenderPass> renderPass, VkExtent2D extent);
		~Pipeline();

		void Release();

	private:
		void Create(Ref<RenderPass> renderPass, VkExtent2D extent);

	private:

		VkPipeline m_Pipeline = nullptr;
		VkPipelineCache m_PipelineCache = nullptr;

		VkPipelineLayout m_PipelineLayout = nullptr;

		VkDevice m_Device = nullptr;

	};


}
