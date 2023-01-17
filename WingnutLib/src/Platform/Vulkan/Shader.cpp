#include "wingnut_pch.h"
#include "Shader.h"

#include "ShaderCompiler.h"

#include "Renderer/Renderer.h"
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
			if (type == "vec2") return VK_FORMAT_R32G32_SFLOAT;
			if (type == "vec3") return VK_FORMAT_R32G32B32_SFLOAT;
			if (type == "vec4") return VK_FORMAT_R32G32B32A32_SFLOAT;

			return VK_FORMAT_R32G32B32A32_SFLOAT;
		}

		uint32_t TypeStringToSize(const std::string& type)
		{
			if (type == "vec2") return 4 * 2;
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
			for (auto& descriptorSetLayout : m_DescriptorSetLayouts)
			{
				if (descriptorSetLayout != nullptr)
				{
					vkDestroyDescriptorSetLayout(m_Device->GetDevice(), descriptorSetLayout, nullptr);
					descriptorSetLayout = nullptr;
				}
			}

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

			AllocateDescriptorSets();
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

				FindUniforms(shaderSource, domain);

			}


		}

		VkDescriptorSetLayout Shader::CreateDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& setBindings)
		{
			VkDescriptorSetLayout newSetLayout = nullptr;

			VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
			layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutCreateInfo.bindingCount = (uint32_t)setBindings.size();
			layoutCreateInfo.pBindings = setBindings.data();

			if (vkCreateDescriptorSetLayout(m_Device->GetDevice(), &layoutCreateInfo, nullptr, &newSetLayout) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Shader] Failed to create shader descriptor set layout");
				return nullptr;
			}

			return newSetLayout;
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
					uint32_t binding = 0;

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

							if (tokens[i] == "binding")
							{
								binding = std::atoi(tokens[++i].c_str());
							}
						}

						VkVertexInputAttributeDescription attributeDescription = {};
						attributeDescription.location = location;
						attributeDescription.binding = binding;
						attributeDescription.format = TypeStringToVulkanFormat(type);
						attributeDescription.offset = m_VertexStride;

						m_AttributeDescriptions.emplace_back(attributeDescription);

						m_VertexStride += TypeStringToSize(type);

//						LOG_CORE_WARN("Layout: location = {}, type = {}, name = {}", location, type, name);
					}
				}
			}
		}

		void Shader::FindUniforms(const std::string& shaderSource, ShaderDomain domain)
		{
			std::istringstream source(shaderSource);
			std::string line;

			while (std::getline(source, line))
			{
				if (line.size() < 2)
				{
					continue;
				}

				if (line.find("uniform") != std::string::npos)
				{
					std::string cleanedLine = RemoveCharacters(line, "();={}");
					std::vector<std::string> tokens = Tokenize(cleanedLine, ' ');

					uint32_t set = 0;
					uint32_t binding = 0;
					
					std::string type;
					std::string name;

					VkDescriptorSetLayoutBinding layoutBinding = {};

					layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					layoutBinding.descriptorCount = 1;
					layoutBinding.pImmutableSamplers = nullptr;

					if (domain == ShaderDomain::Vertex)
					{
						layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
					}
					else if (domain == ShaderDomain::Fragment)
					{
						layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
					}

					for (size_t i = 0; i < tokens.size(); i++)
					{
						if (tokens[i] == "set")
						{
							set = std::atoi(tokens[++i].c_str());
						}

						if (tokens[i] == "binding")
						{
							binding = std::atoi(tokens[++i].c_str());
						}

						if (tokens[i] == "sampler2D")
						{
//							layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
							layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						}
					}

					layoutBinding.binding = binding;



//					LOG_CORE_WARN("Uniform: set = {}, binding = {}", set, binding);

					m_DescriptorSetLayoutBindings[set].emplace_back(layoutBinding);
				}

			}

			for (auto& setLayoutBinding : m_DescriptorSetLayoutBindings)
			{
				bool setExists = false;

				for (auto& setDescription : m_SetDescriptions)
				{
					if (setDescription.SetID == setLayoutBinding.first)
					{
						setExists = true;
					}
				}

				if (!setExists)
				{
					SetDescription setDescription;
					setDescription.SetID = setLayoutBinding.first;

					m_SetDescriptions.emplace_back(setDescription);
				}
			}
		}

		void Shader::AllocateDescriptorSets()
		{
			for (auto& descriptorSetLayoutBindings : m_DescriptorSetLayoutBindings)
			{
				m_DescriptorSetLayouts.emplace_back(CreateDescriptorSetLayout(descriptorSetLayoutBindings.second));
			}

			uint32_t setCount = (uint32_t)m_DescriptorSetLayouts.size();

			if (setCount == 0)
			{
				return;
			}

			VkDescriptorSetAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocateInfo.descriptorPool = Renderer::GetContext()->GetRendererData().DescriptorPool->GetDescriptorPool();
			allocateInfo.descriptorSetCount = setCount;
			allocateInfo.pSetLayouts = m_DescriptorSetLayouts.data();

			m_DescriptorSets.resize(setCount);

			if (vkAllocateDescriptorSets(m_Device->GetDevice(), &allocateInfo, m_DescriptorSets.data()) != VK_SUCCESS)
			{
				LOG_CORE_TRACE("[Shader] Unable to allocate descriptor sets");
				return;
			}

			for (uint32_t i = 0; (uint32_t)i < m_SetDescriptions.size(); i++)
			{
				m_SetDescriptions[i].Set = m_DescriptorSets[i];
			}
		}


		SetDescription Shader::GetDescriptorSet(uint32_t setID)
		{
			for (auto& setDescriptor : m_SetDescriptions)
			{
				if (setDescriptor.SetID == setID)
				{
					return setDescriptor;
				}
			}

			LOG_CORE_ERROR("[Shader] Set {} does not exist in shader");
			return SetDescription();
		}

	}

}
