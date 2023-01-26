#pragma once

#include "Device.h"
#include "DescriptorPool.h"

#include <vulkan/vulkan.h>



namespace Wingnut
{

	namespace Vulkan
	{


		enum class ShaderDomain
		{
			None,
			Vertex,
			Fragment,

			Raytrace,

			Compute,
		};

		struct ShaderModule
		{
			VkShaderModule Module = nullptr;
			ShaderDomain Domain = ShaderDomain::None;
		};


		struct SetDescription
		{
			uint32_t SetID = -1;
			VkDescriptorSet Set = nullptr;
		};


		class Shader
		{
		public:
			Shader(Ref<Device> device, const std::string& shaderPath);
			Shader(Ref<Device> device, const std::string& shaderPath, ShaderDomain domain);
			~Shader();

			void Release();

			void Reload();

			std::vector<ShaderModule>& GetShaderModules() { return m_ShaderModules; }

			std::vector<VkVertexInputAttributeDescription>& GetAttributeDescriptions() { return m_AttributeDescriptions; }
			uint32_t GetVertexStride() const { return m_VertexStride; }

			std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayouts() { return m_DescriptorSetLayouts; }

			std::vector<VkDescriptorSet>& GetDescriptorSets() { return m_DescriptorSets; }

			SetDescription GetDescriptorSet(uint32_t setID);

			void UpdateDescriptorSet(uint32_t set, uint32_t binding, VkBuffer buffer, uint32_t bufferSize);
			void UpdateDescriptorSet(uint32_t set, uint32_t binding, VkImageView image, VkSampler sampler);

			void BindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);

			void SetAttributeFormat(uint32_t location, VkFormat format);

		private:

			void LoadSources();
			void Compile();
			void Reflect();

			void GetVertexLayout(const std::string& shaderSource);
			void FindUniforms(const std::string& shaderSource, ShaderDomain domain);

			void AllocateDescriptorSets();

			VkDescriptorSetLayout CreateDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& setBindings);

		private:

			Ref<Device> m_Device;

			std::string m_ShaderPath;
			ShaderDomain m_Domain;

			std::vector<uint32_t> m_Data;

			std::unordered_map<ShaderDomain, std::string> m_Sources;

			std::vector<ShaderModule> m_ShaderModules;

			std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions;
			uint32_t m_VertexStride = 0;

			std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> m_DescriptorSetLayoutBindings;
			std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts;

			std::vector<VkDescriptorSet> m_DescriptorSets;

			std::vector<SetDescription> m_SetDescriptions;

			std::vector<VkWriteDescriptorSet> m_DescriptorSetData;
		};


	}

}
