#include "wingnut_pch.h"
#include "Shader.h"

#include "ShaderCompiler.h"


namespace Wingnut
{

	namespace Vulkan
	{

		Shader::Shader(const std::string& shaderPath, ShaderDomain domain)
			: m_ShaderPath(shaderPath)
		{
			Reload();
		}

		Shader::~Shader()
		{

		}


		void Shader::Reload()
		{
			LoadSource();

			Compile();

			Reflect();
		}

		void Shader::LoadSource()
		{
			std::ifstream file(m_ShaderPath, std::ios::in | std::ios::binary);

			if (!file.is_open())
			{
				LOG_CORE_ERROR("[Shader] Unable to load file {}", m_ShaderPath);
				return;
			}

			file.seekg(0, file.end);
			uint32_t fileSize = (uint32_t)file.tellg();
			file.seekg(file.beg);

			m_Source.resize(fileSize);
			file.read((char*)m_Source.data(), fileSize);

			file.close();	
		}

		void Shader::Compile()
		{
			ShaderCompiler::Initialize();

			auto [resultDomain, shaderData] = ShaderCompiler::Compile(m_ShaderPath, m_Domain);

			ShaderCompiler::Shutdown();

			if (resultDomain == ShaderDomain::None)
			{
				LOG_CORE_ERROR("[Pipeline] Failed to compile shader");

				return;
			}

			m_Data = shaderData;

//			CreateShaderModule(resultDomain, shaderData);

		}


		void Shader::Reflect()
		{
			
		}


	}

}
