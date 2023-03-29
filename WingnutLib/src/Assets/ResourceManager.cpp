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

		for (auto& materialIterator : s_Materials)
		{
			auto& materialVector = materialIterator.second;

			for (auto& material : materialVector)
			{
				if (material->GetID() == materialID)
				{
					return material;
				}
			}
		}

		return nullptr;
	}

	Ref<Material> ResourceManager::GetMaterial(MaterialType type, UUID materialID)
	{
		auto& materialVector = s_Materials[type];

		for (auto& material : materialVector)
		{
			if (material->GetID() == materialID)
			{
				return material;
			}
		}

		return nullptr;
	}

	void ResourceManager::StoreMaterial(Ref<Material> material)
	{
		auto& materialVector = s_Materials[material->GetType()];

		for (auto& iterator : materialVector)
		{
			if (iterator->GetID() == material->GetID())
			{
				LOG_CORE_TRACE("[ResourceManager] Material {} already exists in store", material->GetName());
				return;
			}
		}

		s_Materials[material->GetType()].emplace_back(material);
	}

	void ResourceManager::ClearMaterials()
	{
		for (auto& materialType : s_Materials)
		{
			for (auto& material : materialType.second)
			{
				material->Release();
			}
		}

		s_Materials.clear();
	}

	Ref<Material> ResourceManager::GetMaterialByName(const std::string& name)
	{
		for (auto& materialInstance : s_Materials)
		{
			auto& materialVector = materialInstance.second;

			for (auto& material : materialVector)
			{
				if (material->GetName() == name)
				{
					return material;
				}
			}

		}

		return nullptr;
	}

	Ref<Material> ResourceManager::GetMaterialByName(MaterialType type, const std::string& name)
	{
		auto& materialVector = s_Materials[type];

		for (auto& material : materialVector)
		{
			if (material->GetName() == name)
			{
				return material;
			}
		}

		return nullptr;
	}

	void ResourceManager::DeleteMaterial(MaterialType type, const std::string& materialName)
	{
		for (auto iterator = s_Materials[type].begin(); iterator != s_Materials[type].end(); iterator++)
		{
			if ((*iterator)->GetName() == materialName)
			{
				s_Materials[type].erase(iterator);
				return;
			}
		}

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
		if (s_Textures.find(texture->GetTextureID()) == s_Textures.end())
		{
			s_Textures[texture->GetTextureID()] = texture;
		}
	}

	bool ResourceManager::FindTexture(const std::string& textureName)
	{
		for (auto& containerIterator : s_Textures)
		{
			if (containerIterator.second->GetTextureName() == textureName)
			{
				return true;
			}
		}

		return false;
	}


	Ref<Vulkan::Texture2D> ResourceManager::GetTexture(UUID textureID)
	{
		if (s_Textures.find(textureID) != s_Textures.end())
		{
			return s_Textures[textureID];
		}

		return nullptr;
	}
	
	Ref<Vulkan::Texture2D> ResourceManager::GetTextureByName(const std::string& textureName)
	{
		for (auto containerIterator : s_Textures)
		{
			if (containerIterator.second->GetTextureName() == textureName)
			{
				return containerIterator.second;
			}
		}

		return nullptr;
	}

	void ResourceManager::ClearTextures()
	{
		for (auto& texture : s_Textures)
		{
			texture.second->Release();
		}

		s_Textures.clear();
	}


}
