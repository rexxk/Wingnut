#include "wingnut_pch.h"
#include "Shader.h"

#include "ShaderCompiler.h"


namespace Wingnut
{

	namespace Vulkan
	{

		ShaderDomain DomainStringToShaderDomain(const std::string_view& domainString)
		{
			if (domainString == "vertex") return ShaderDomain::Vertex;
			if (domainString == "fragment") return ShaderDomain::Fragment;
			if (domainString == "pixel") return ShaderDomain::Fragment;
			if (domainString == "compute") return ShaderDomain::Compute;
			if (domainString == "raytrace") return ShaderDomain::Raytrace;
			if (domainString == "rt") return ShaderDomain::Raytrace;

			return ShaderDomain::None;
		}


		Shader::Shader(Ref<Device> device, const std::string& shaderPath)
			: m_ShaderPath(shaderPath), m_Device(device)
		{
			Reload();
		}

		Shader::Shader(Ref<Device> device, const std::string& shaderPath, ShaderDomain domain)
			: m_ShaderPath(shaderPath), m_Device(device)
		{
			Reload();
		}

		Shader::~Shader()
		{
			Release();
		}

		void Shader::Release()
		{
			for (auto& shaderModule : m_ShaderModules)
			{
				vkDestroyShaderModule(m_Device->GetDevice(), shaderModule.Module, nullptr);
			}

			m_ShaderModules.clear();
		}

		void Shader::Reload()
		{
			LoadSources();

			Compile();

			Reflect();
		}

		void Shader::LoadSources()
		{
			// Load shader file

			std::ifstream file(m_ShaderPath, std::ios::in | std::ios::binary);

			if (!file.is_open())
			{
				LOG_CORE_ERROR("[Shader] Unable to load file {}", m_ShaderPath);
				return;
			}

			file.seekg(0, file.end);
			uint32_t fileSize = (uint32_t)file.tellg();
			file.seekg(file.beg);

			std::string shaderFile;
			shaderFile.resize(fileSize + 1);

			file.read((char*)shaderFile.data(), fileSize);
			shaderFile[fileSize] = 0;

			file.close();

			// Parse shader file and extract shaders

			size_t offset = 0;
			size_t location = 0;

			std::string typeString = "#type";
			size_t typeLength = typeString.length() + 1;

			while ((location = shaderFile.find(typeString, offset)) != std::string::npos)
			{
				size_t endPosition = shaderFile.find("\r\n", location);
				std::string domain = shaderFile.substr(location + typeLength, endPosition - (location + typeLength));

				// Extract shader data
				size_t shaderStart = endPosition + 1;
				size_t shaderEnd = shaderFile.find(typeString, endPosition);

				std::string shaderSource;

				if (shaderEnd == std::string::npos)
				{
					shaderSource = shaderFile.substr(shaderStart);
				}
				else
				{
					shaderSource = shaderFile.substr(shaderStart, shaderEnd - shaderStart);
				}

				m_Sources[DomainStringToShaderDomain(domain)] = shaderSource;

				offset = endPosition;
			}
		}

		void Shader::Compile()
		{
			ShaderCompiler::Initialize();

			for (auto& sourceEntry : m_Sources)
			{
				ShaderDomain domain = sourceEntry.first;
				std::string& sourceCode = sourceEntry.second;

				auto [resultDomain, shaderData] = ShaderCompiler::Compile(sourceCode, domain);

				if (resultDomain == ShaderDomain::None)
				{
					LOG_CORE_ERROR("[Pipeline] Failed to compile shader");
					return;
				}

				ShaderModule shaderModule;

				shaderModule.Domain = resultDomain;

				VkShaderModuleCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.codeSize = shaderData.size() * sizeof(uint32_t);
				createInfo.pCode = shaderData.data();

				if (vkCreateShaderModule(m_Device->GetDevice(), &createInfo, nullptr, &shaderModule.Module) != VK_SUCCESS)
				{
					LOG_CORE_ERROR("[Shader] Failed to create shader module");
					return;
				}

				m_ShaderModules.emplace_back(shaderModule);
			}

			ShaderCompiler::Shutdown();

		}


		void Shader::Reflect()
		{
			
		}


	}

}
