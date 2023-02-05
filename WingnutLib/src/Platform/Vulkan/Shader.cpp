#include "wingnut_pch.h"
#include "Shader.h"

#include "ShaderCompiler.h"

#include "Buffer.h"

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
			if (type == "uint") return VK_FORMAT_R8G8B8A8_UNORM;

			return VK_FORMAT_R32G32B32A32_SFLOAT;
		}

		uint32_t TypeStringToSize(const std::string& type)
		{
			if (type == "vec2") return 4 * 2;
			if (type == "vec3") return 4 * 3;
			if (type == "vec4") return 4 * 4;
			if (type == "uint") return 4;

			return 4;
		}

		Ref<Shader> Shader::Create(Ref<Device> device, const std::string& shaderPath)
		{
			return CreateRef<Shader>(device, shaderPath);
		}


		Shader::Shader(Ref<Device> device, const std::string& shaderPath)
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
				if (descriptorSetLayout.second != nullptr)
				{
					vkDestroyDescriptorSetLayout(m_Device->GetDevice(), descriptorSetLayout.second, nullptr);
					descriptorSetLayout.second = nullptr;
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

			for (auto& descriptorSetLayoutBindings : m_DescriptorSetLayoutBindings)
			{
				m_DescriptorSetLayouts[descriptorSetLayoutBindings.first] = CreateDescriptorSetLayout(descriptorSetLayoutBindings.second);
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

		VkDescriptorSetLayout Shader::GetDescriptorSetLayout(uint32_t set)
		{
			if (m_DescriptorSetLayouts.find(set) != m_DescriptorSetLayouts.end())
			{
				return m_DescriptorSetLayouts[set];
			}
			
			return nullptr;
		}

		void Shader::GetVertexLayout(const std::string& shaderSource)
		{
			std::istringstream source(shaderSource);
			std::string line;

			m_VertexStride = 0;

			bool overrideNext = false;
			std::string overrideType = "";

			while (std::getline(source, line))
			{
				if (line.size() < 2)
				{
					continue;
				}

				if (line.find("// override") != std::string::npos)
				{
					std::string cleanedString = RemoveCharacters(line, "();=");
					std::vector<std::string> tokens = Tokenize(cleanedString, ' ');

					overrideNext = true;
					overrideType = tokens[2];
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

								if (overrideNext)
								{
									type = overrideType;
									overrideNext = false;
								}
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
		}

		void Shader::SetAttributeFormat(uint32_t location, VkFormat format)
		{
			if (location > (uint32_t)m_AttributeDescriptions.size())
				return;

			m_AttributeDescriptions[location].format = format;
		}


		///////////////////////////////////////////////////

		Ref<Descriptor> Descriptor::Create(Ref<Device> device, Ref<Shader> shader, uint32_t set, uint32_t binding, Ref<UniformBuffer> buffer)
		{
			return CreateRef<Descriptor>(device, shader, set, binding, buffer);
		}

		Ref<Descriptor> Descriptor::Create(Ref<Device> device, Ref<Shader> shader, Ref<ImageSampler> sampler, uint32_t set, uint32_t binding, Ref<Texture2D> texture)
		{
			return CreateRef<Descriptor>(device, shader, sampler, set, binding, texture);
		}




		Descriptor::Descriptor(Ref<Device> device, Ref<Shader> shader, uint32_t set, uint32_t binding, Ref<UniformBuffer> buffer)
			: m_Device(device), m_Shader(shader), m_Type(DescriptorType::DataBuffer), m_Set(set), m_Binding(binding)
		{
			Allocate();

			CreateDescriptor(device, shader, set, binding, buffer);
		}

		Descriptor::Descriptor(Ref<Device> device, Ref<Shader> shader, Ref<ImageSampler> sampler, uint32_t set, uint32_t binding, Ref<Texture2D> texture)
			: m_Device(device), m_Shader(shader), m_Type(DescriptorType::Texture), m_Set(set), m_Binding(binding), m_Sampler(sampler)
		{
			Allocate();

			CreateDescriptor(device, shader, sampler, set, binding, texture);
		}

		Descriptor::~Descriptor()
		{
			Release();
		}

		void Descriptor::Release()
		{
			
		}

		void Descriptor::Allocate()
		{
			VkDescriptorSetAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocateInfo.descriptorPool = Renderer::GetContext()->GetRendererData().DescriptorPool->GetDescriptorPool();
			allocateInfo.descriptorSetCount = 1;

			VkDescriptorSetLayout layouts[] = { m_Shader->GetDescriptorSetLayout(m_Set) };
			allocateInfo.pSetLayouts = layouts;

			if (vkAllocateDescriptorSets(m_Device->GetDevice(), &allocateInfo, &m_Descriptor) != VK_SUCCESS)
			{
				LOG_CORE_TRACE("[Shader] Unable to allocate descriptor sets");
				return;
			}
		}

		void Descriptor::CreateDescriptor(Ref<Device> device, Ref<Shader> shader, uint32_t set, uint32_t binding, Ref<UniformBuffer> buffer)
		{
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = buffer->GetBuffer(Renderer::GetContext()->GetCurrentFrame());
			bufferInfo.offset = 0;
			bufferInfo.range = buffer->GetBufferSize();

			VkWriteDescriptorSet writeSet = {};
			writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeSet.dstBinding = m_Binding;
			writeSet.dstSet = m_Descriptor;
			writeSet.descriptorCount = 1;
			writeSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			writeSet.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(m_Device->GetDevice(), 1, &writeSet, 0, nullptr);

		}

		void Descriptor::CreateDescriptor(Ref<Device> device, Ref<Shader> shader, Ref<ImageSampler> sampler, uint32_t set, uint32_t binding, Ref<Texture2D> texture)
		{
			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = texture->GetImageView();
			imageInfo.sampler = sampler->Sampler();

			VkWriteDescriptorSet writeSet = {};
			writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeSet.dstBinding = m_Binding;
			writeSet.dstSet = m_Descriptor;
			writeSet.descriptorCount = 1;
			writeSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writeSet.pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(m_Device->GetDevice(), 1, &writeSet, 0, nullptr);
		}

		void Descriptor::Bind(Ref<CommandBuffer> commandBuffer, VkPipelineLayout pipelineLayout)
		{
			vkCmdBindDescriptorSets(commandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, m_Set, 1, &m_Descriptor, 0, nullptr);
		}

		void Descriptor::UpdateDescriptor(Ref<Texture2D> texture)
		{
			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = texture->GetImageView();
			imageInfo.sampler = m_Sampler->Sampler();

			VkWriteDescriptorSet writeSet = {};
			writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeSet.dstBinding = m_Binding;
			writeSet.dstSet = m_Descriptor;
			writeSet.descriptorCount = 1;
			writeSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writeSet.pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(m_Device->GetDevice(), 1, &writeSet, 0, nullptr);
		}


	}

}
