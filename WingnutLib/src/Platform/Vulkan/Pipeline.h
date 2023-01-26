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


		enum class CullMode
		{
			None,
			Back,
			Front,
			FrontAndBack,
		};

		enum class CullingDirection
		{
			Clockwise,
			CounterClockwise,
		};


		struct PipelineSpecification
		{
			Ref<Shader> PipelineShader;
			Ref<RenderPass> RenderPass;
//			PolygonFillType FillType = PolygonFillType::Solid;
			float LineWidth = 1.0f;

			CullMode CullMode;
			CullingDirection CullingDirection;

			VkExtent2D Extent;
		};



		class Pipeline
		{
		public:
			Pipeline(Ref<Device> device, const PipelineSpecification& specification);
			~Pipeline();

			void Release();

			PipelineSpecification& GetSpecification() { return m_Specification; }

			VkPipeline GetPipeline() { return m_Pipeline; }

			VkPipelineLayout& GetLayout() { return m_PipelineLayout; }

		private:
			void Create();

		private:

			VkPipeline m_Pipeline = nullptr;
			VkPipelineCache m_PipelineCache = nullptr;

			VkPipelineLayout m_PipelineLayout = nullptr;

			VkDevice m_Device = nullptr;

			PipelineSpecification m_Specification;
		};


	}
}
