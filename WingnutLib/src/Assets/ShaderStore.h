#pragma once

#include "Platform/Vulkan/Shader.h"


namespace Wingnut
{

	enum class ShaderType
	{
		Default,
		ImGui,
	};


	class ShaderStore
	{
	public:
		static void LoadShader(ShaderType type , const std::string& shaderPath);

		static Ref<Vulkan::Shader> GetShader(ShaderType type);

		static void Release();

	private:
		static inline std::unordered_map<ShaderType, Ref<Vulkan::Shader>> s_Shaders;
	};


}
