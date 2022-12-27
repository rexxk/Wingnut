#include "wingnut_pch.h"
#include "Shader.h"

#include "ShaderCompiler.h"

#include "Utils/StringUtils.h"


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

		VkFormat TypeStringToVulkanFormat(const std::string& type)
		{
			if (type == "vec3") return VK_FORMAT_R32G32B32_SFLOAT;
			if (type == "vec4") return VK_FORMAT_R32G32B32A32_SFLOAT;

			return VK_FORMAT_R32G32B32A32_SFLOAT;
		}

		uint32_t TypeStringToSize(const std::string& type)
		{
			if (type == "vec3") return 4 * 3;
			if (type == "vec4") return 4 * 4;

			return 4;
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
			for (auto& shader : m_Sources)
			{
				ShaderDomain domain = shader.first;
				std::string shaderSource = shader.second;

				if (domain == ShaderDomain::Vertex)
				{
					GetVertexLayout(shaderSource);
				}


			}
		}

		void Shader::GetVertexLayout(const std::string& shaderSource)
		{
			std::istringstream source(shaderSource);
			std::string line;

			m_VertexStride = 0;

			while (std::getline(source, line))
			{
				if (line.size() < 2)
				{
					continue;
				}

				if (line.find("layout") != std::string::npos)
				{
					bool validInput = false;

					std::string cleanedLine = RemoveCharacters(line, "();=");
					std::vector<std::string> tokens = Tokenize(cleanedLine, ' ');

					uint32_t location = -1;

					std::string type;
					std::string name;

					for (auto& token : tokens)
					{
						if (token == "in")
						{
							validInput = true;
						}
					}

					if (validInput == true)
					{
						for (size_t i = 0; i < tokens.size(); i++)
						{
							if (tokens[i] == "location")
							{
								location = std::atoi(tokens[++i].c_str());
							}

							if (tokens[i] == "in")
							{
								type = tokens[++i];
								name = tokens[++i];
							}
						}

						VkVertexInputAttributeDescription attributeDescription = {};
						attributeDescription.location = location;
						attributeDescription.binding = 0;
						attributeDescription.format = TypeStringToVulkanFormat(type);
						attributeDescription.offset = m_VertexStride;

						m_AttributeDescriptions.emplace_back(attributeDescription);

						m_VertexStride += TypeStringToSize(type);

//						LOG_CORE_WARN("Layout: location = {}, type = {}, name = {}", location, type, name);
					}
				}
			}
		}


	}

}
