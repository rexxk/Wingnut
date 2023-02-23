#include "wingnut_pch.h"
#include "ShaderStore.h"

#include "Renderer/Renderer.h"

namespace Wingnut
{

	std::string ShaderTypeToString(ShaderType type)
	{
		switch (type)
		{
		case ShaderType::Default: return "Default";
		case ShaderType::ImGui: return "ImGui";
		}

		return "<unknown>";
	}


	void ShaderStore::LoadShader(ShaderType type, const std::string& shaderPath)
	{
		if (s_Shaders.find(type) != s_Shaders.end())
		{
			LOG_CORE_ERROR("[ShaderStore] Shader {} already set in ShaderStore", ShaderTypeToString(type));
			return;
		}

		s_Shaders[type] = Vulkan::Shader::Create(Renderer::GetContext()->GetRendererData().Device, shaderPath);
	}

	Ref<Vulkan::Shader> ShaderStore::GetShader(ShaderType type)
	{
		if (s_Shaders.find(type) != s_Shaders.end())
		{
			return s_Shaders[type];
		}

		return nullptr;
	}

	void ShaderStore::Release()
	{
		for (auto& shader : s_Shaders)
		{
			shader.second->Release();
		}
	}

}
