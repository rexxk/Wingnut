#include "wingnut_pch.h"
#include "ShaderStore.h"

#include "Renderer/Renderer.h"

namespace Wingnut
{

	void ShaderStore::LoadShader(const std::string& name, const std::string& shaderPath)
	{
		if (s_Shaders.find(name) != s_Shaders.end())
		{
			LOG_CORE_ERROR("[ShaderStore] Shader {} already exists in ShaderStore", name);
			return;
		}

		s_Shaders[name] = Vulkan::Shader::Create(Renderer::GetContext()->GetRendererData().Device, shaderPath);
	}

	Ref<Vulkan::Shader> ShaderStore::GetShader(const std::string& name)
	{
		if (s_Shaders.find(name) != s_Shaders.end())
		{
			return s_Shaders[name];
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
