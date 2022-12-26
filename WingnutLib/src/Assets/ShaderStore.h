#pragma once

#include "Platform/Vulkan/Shader.h"


namespace Wingnut
{


	class ShaderStore
	{
	public:
		static void LoadShader(const std::string& name, const std::string& shaderPath);

		static Ref<Vulkan::Shader> GetShader(const std::string& name);

	private:
		static inline std::unordered_map<std::string, Ref<Vulkan::Shader>> s_Shaders;
	};


}
