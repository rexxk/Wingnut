#pragma once

#include "Device.h"
#include "RenderPass.h"

#include <vulkan/vulkan.h>



namespace Wingnut
{


	class Pipeline
	{
	public:
		Pipeline(Ref<Device> device, Ref<RenderPass> renderPass);
		~Pipeline();

		void Release();

	private:
		void Create(Ref<RenderPass> renderPass);

	private:

		VkPipeline m_Pipeline = nullptr;
		VkPipelineCache m_PipelineCache = nullptr;

		VkDevice m_Device = nullptr;

	};


}
