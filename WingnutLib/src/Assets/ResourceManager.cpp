#include "wingnut_pch.h"
#include "ResourceManager.h"



namespace Wingnut
{


	void ResourceManager::LoadMaterial(const std::string& materialPath)
	{
		LOG_CORE_ERROR("[MaterialStore] Loading is not implemented");
	}

	Ref<Material> ResourceManager::GetMaterial(UUID materialID)
	{
		if (s_Materials.find(materialID) != s_Materials.end())
		{
			return s_Materials[materialID];
		}

		return nullptr;
	}

	void ResourceManager::StoreMaterial(Ref<Material> material)
	{
		s_Materials[material->GetID()] = material;
	}

	void ResourceManager::ClearMaterials()
	{
		for (auto& material : s_Materials)
		{
			material.second->Release();
		}

		s_Materials.clear();
	}

	Ref<Material> ResourceManager::GetMaterialByName(const std::string& name)
	{
		for (auto& materialInstance : s_Materials)
		{
			auto& material = materialInstance.second;

			if (material->GetName() == name)
			{
				return material;
			}
		}

		return nullptr;
	}

	void ResourceManager::DeleteMaterial(UUID materialID)
	{
		s_Materials.erase(materialID);
	}

	std::string ResourceManager::SamplerTypeToString(SamplerType type)
	{
		switch (type)
		{
		case SamplerType::Default: return "Default";
		case SamplerType::LinearClamp: return "LinearClamp";
		case SamplerType::LinearRepeat: return "LinearRepeat";
		case SamplerType::NearestRepeat: return "NearestRepeat";
		}

		return "<unknown>";
	}

	SamplerType ResourceManager::GetSamplerTypeByName(const std::string& samplerName)
	{
		if (samplerName == "Default") return SamplerType::Default;
		if (samplerName == "LinearClamp") return SamplerType::LinearClamp;
		if (samplerName == "LinearRepeat") return SamplerType::LinearRepeat;
		if (samplerName == "NearestRepeat") return SamplerType::NearestRepeat;

		return SamplerType::Default;
	}

	void ResourceManager::AddSampler(SamplerType type, Ref<Vulkan::ImageSampler> sampler)
	{
		if (s_Samplers.find(type) != s_Samplers.end())
		{
			LOG_CORE_ERROR("[SamplerStore] Sampler {} already set in SamplerStore", SamplerTypeToString(type));
			return;
		}

		LOG_CORE_TRACE("[SamplerStore] Adding sampler {}", SamplerTypeToString(type));

		s_Samplers[type] = sampler;
	}

	Ref<Vulkan::ImageSampler> ResourceManager::GetSampler(SamplerType type)
	{
		if (type == SamplerType::Default)
			type = SamplerType::NearestRepeat;

		if (s_Samplers.find(type) != s_Samplers.end())
		{
			return s_Samplers[type];
		}

		return nullptr;
	}

	void ResourceManager::Release()
	{
		for (auto& sampler : s_Samplers)
		{
			sampler.second->Release();
		}

		for (auto& shader : s_Shaders)
		{
			shader.second->Release();
		}
	}

	std::string ShaderTypeToString(ShaderType type)
	{
		switch (type)
		{
		case ShaderType::Default: return "Default";
		case ShaderType::ImGui: return "ImGui";
		}

		return "<unknown>";
	}


	void ResourceManager::LoadShader(ShaderType type, const std::string& shaderPath)
	{
		if (s_Shaders.find(type) != s_Shaders.end())
		{
			LOG_CORE_ERROR("[ShaderStore] Shader {} already set in ShaderStore", ShaderTypeToString(type));
			return;
		}

		s_Shaders[type] = Vulkan::Shader::Create(Renderer::GetContext()->GetRendererData().Device, shaderPath);
	}

	Ref<Vulkan::Shader> ResourceManager::GetShader(ShaderType type)
	{
		if (s_Shaders.find(type) != s_Shaders.end())
		{
			return s_Shaders[type];
		}

		return nullptr;
	}

	void ResourceManager::AddTexture(Ref<Vulkan::Texture2D> texture)
	{
		if (m_Textures.find(texture->GetTextureID()) == m_Textures.end())
		{
			m_Textures[texture->GetTextureID()] = texture;
		}
	}

	void ResourceManager::AddDescriptor(UUID textureID, Ref<Vulkan::Descriptor> descriptor)
	{
		if (m_UITextureDescriptors.find(textureID) == m_UITextureDescriptors.end())
		{
			m_UITextureDescriptors[textureID] = descriptor;
		}
	}

	void ResourceManager::AddTextureData(Ref<Vulkan::Texture2D> texture, Ref<Vulkan::Descriptor> descriptor)
	{
		AddTexture(texture);
		AddDescriptor(texture->GetTextureID(), descriptor);
	}

	bool ResourceManager::FindTexture(const std::string& textureName)
	{
		for (auto& containerIterator : m_Textures)
		{
			if (containerIterator.second->GetTextureName() == textureName)
			{
				return true;
			}
		}

		return false;
	}


	void ResourceManager::SetDescriptor(UUID textureID, Ref<Vulkan::Descriptor> descriptor)
	{
		if (m_UITextureDescriptors.find(textureID) != m_UITextureDescriptors.end())
		{
			m_UITextureDescriptors[textureID] = descriptor;
		}
	}

	std::pair<Ref<Vulkan::Texture2D>, Ref<Vulkan::Descriptor>> ResourceManager::GetTextureData(UUID textureID)
	{
		return std::make_pair(GetTexture(textureID), GetDescriptor(textureID));
	}

	Ref<Vulkan::Texture2D> ResourceManager::GetTexture(UUID textureID)
	{
		if (m_Textures.find(textureID) != m_Textures.end())
		{
			return m_Textures[textureID];
		}

		return nullptr;
	}

	Ref<Vulkan::Descriptor> ResourceManager::GetDescriptor(UUID textureID)
	{
		if (m_UITextureDescriptors.find(textureID) != m_UITextureDescriptors.end())
		{
			return m_UITextureDescriptors[textureID];
		}

		return nullptr;
	}

	void ResourceManager::ClearTextures()
	{
		for (auto& texture : m_Textures)
		{
			texture.second->Release();
		}

		m_Textures.clear();
		m_UITextureDescriptors.clear();
	}


}
