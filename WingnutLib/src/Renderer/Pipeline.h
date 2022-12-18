#pragma once

#include "Device.h"
#include "RenderPass.h"

#include "ShaderCompiler.h"

#include <vulkan/vulkan.h>



namespace Wingnut
{


	class Pipeline
	{
	public:
		Pipeline(Ref<Device> device, Ref<RenderPass> renderPass, VkExtent2D extent, const std::unordered_map<ShaderDomain, std::string>& shaderPaths);
		~Pipeline();

		void Release();

		VkPipeline GetPipeline() { return m_Pipeline; }

	private:
		void CompileShaders(const std::unordered_map<ShaderDomain, std::string>& shaderPaths);
		void CreateShaderModule(ShaderDomain domain, const std::vector<uint32_t>& shaderData);
		void Create(Ref<RenderPass> renderPass, VkExtent2D extent);

	private:

		VkPipeline m_Pipeline = nullptr;
		VkPipelineCache m_PipelineCache = nullptr;

		VkPipelineLayout m_PipelineLayout = nullptr;

		VkDevice m_Device = nullptr;


		std::unordered_map<ShaderDomain, std::vector<uint32_t>> m_ShaderBinaries;
		std::unordered_map<ShaderDomain, VkShaderModule> m_ShaderModules;

	};


}
