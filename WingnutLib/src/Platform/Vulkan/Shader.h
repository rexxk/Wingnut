#pragma once

#include "CommandBuffer.h"
#include "Device.h"
#include "DescriptorPool.h"

#include "Texture.h"

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

		class Shader
		{
			friend class Descriptor;

		public:
			static Ref<Shader> Create(Ref<Device> device, const std::string& shaderPath);

			Shader(Ref<Device> device, const std::string& shaderPath);
			~Shader();

			void Release();

			void Reload();

			std::vector<ShaderModule>& GetShaderModules() { return m_ShaderModules; }

			std::vector<VkVertexInputAttributeDescription>& GetAttributeDescriptions() { return m_AttributeDescriptions; }
			uint32_t GetVertexStride() const { return m_VertexStride; }

			VkDescriptorSetLayout GetDescriptorSetLayout(uint32_t set);
			std::unordered_map<uint32_t, VkDescriptorSetLayout> GetDescriptorSetLayouts() {	return m_DescriptorSetLayouts; }

			void SetAttributeFormat(uint32_t location, VkFormat format);

		private:

			void LoadSources();
			void Compile();
			void Reflect();

			void GetVertexLayout(const std::string& shaderSource);
			void FindUniforms(const std::string& shaderSource, ShaderDomain domain);

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
			std::unordered_map<uint32_t, VkDescriptorSetLayout> m_DescriptorSetLayouts;

		};



		enum class DescriptorType
		{
			DataBuffer,
			Texture,
		};

		class UniformBuffer;

		class Descriptor
		{
		public:
			static Ref<Descriptor> Create(Ref<Device> device, Ref<Shader> shader, uint32_t set);

			Descriptor(Ref<Device> device, Ref<Shader> shader, uint32_t set);
			~Descriptor();

			void Release();

			void Bind(Ref<CommandBuffer> commandBuffer, VkPipelineLayout pipelineLayout);
			void UpdateBindings();

			void SetBufferBinding(uint32_t binding, Ref<UniformBuffer> buffer);
			void SetImageBinding(uint32_t binding, Ref<Texture2D> texture, Ref<ImageSampler> sampler);

			VkDescriptorSet GetDescriptor() { return m_Descriptor; }

		private:
			void Allocate();
			void SetupBindingLayout();

		private:

			uint32_t m_Set = 0;
			uint32_t m_BindingCount = 0;

			VkDescriptorSet m_Descriptor = nullptr;

			DescriptorType m_Type;

			Ref<Device> m_Device = nullptr;
			Ref<Shader> m_Shader = nullptr;

			std::unordered_map<uint32_t, VkDescriptorBufferInfo> m_BufferInfoMap;
			std::unordered_map<uint32_t, VkDescriptorImageInfo> m_ImageInfoMap;

			std::vector<VkWriteDescriptorSet> m_WriteInfo;
		};


	}

}
