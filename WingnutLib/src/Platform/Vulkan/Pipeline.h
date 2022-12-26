#pragma once

#include "Device.h"
#include "RenderPass.h"
#include "Shader.h"

#include "ShaderCompiler.h"

#include <vulkan/vulkan.h>



namespace Wingnut
{

	namespace Vulkan
	{

		struct PipelineSpecification
		{
			Ref<Shader> PipelineShader;

//			PolygonFillType FillType = PolygonFillType::Solid;
			float LineWidth = 1.0f;

			VkExtent2D Extent;
		};



		class Pipeline
		{
		public:
			Pipeline(Ref<Device> device, Ref<RenderPass> renderPass, const PipelineSpecification& specification);
			~Pipeline();

			void Release();

			PipelineSpecification& GetSpecification() { return m_Specification; }

			VkPipeline GetPipeline() { return m_Pipeline; }

		private:
			void Create(Ref<RenderPass> renderPass);

		private:

			VkPipeline m_Pipeline = nullptr;
			VkPipelineCache m_PipelineCache = nullptr;

			VkPipelineLayout m_PipelineLayout = nullptr;

			VkDevice m_Device = nullptr;

			PipelineSpecification m_Specification;
		};


	}
}
