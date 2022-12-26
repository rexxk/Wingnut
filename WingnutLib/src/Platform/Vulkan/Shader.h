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

		private:

			void LoadSources();
			void Compile();
			void Reflect();


		private:

			Ref<Device> m_Device;

			std::string m_ShaderPath;
			ShaderDomain m_Domain;

			std::vector<uint32_t> m_Data;

			std::unordered_map<ShaderDomain, std::string> m_Sources;

			std::vector<ShaderModule> m_ShaderModules;
		};


	}

}
