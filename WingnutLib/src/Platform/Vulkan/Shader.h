#pragma once

#include "Device.h"

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
		public:
			Shader(Ref<Device> device, const std::string& shaderPath);
			Shader(Ref<Device> device, const std::string& shaderPath, ShaderDomain domain);
			~Shader();

			void Release();

			void Reload();

			std::vector<ShaderModule>& GetShaderModules() { return m_ShaderModules; }

			std::vector<VkVertexInputAttributeDescription>& GetAttributeDescriptions() { return m_AttributeDescriptions; }
			uint32_t GetVertexStride() const { return m_VertexStride; }

		private:

			void LoadSources();
			void Compile();
			void Reflect();

			void GetVertexLayout(const std::string& shaderSource);

		private:

			Ref<Device> m_Device;

			std::string m_ShaderPath;
			ShaderDomain m_Domain;

			std::vector<uint32_t> m_Data;

			std::unordered_map<ShaderDomain, std::string> m_Sources;

			std::vector<ShaderModule> m_ShaderModules;

			std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions;
			uint32_t m_VertexStride = 0;

		};


	}

}
