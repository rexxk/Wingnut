#pragma once


#include "Shader.h"



namespace Wingnut
{

	namespace Vulkan
	{

		class ShaderCompiler
		{
		public:
			static void Initialize();
			static void Shutdown();

			static std::pair<ShaderDomain, std::vector<uint32_t>> Compile(const std::string& shaderPath, ShaderDomain domain);

		};

	}

}
