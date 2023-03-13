#pragma once

#include "Renderer/PBRMaterial.h"
#include "Renderer/UIMaterial.h"


namespace Wingnut
{


	class ResourceManager
	{
	public:

		static void Release();

//		Material
		static void LoadMaterial(const std::string& materialPath);

		static Ref<Material> GetMaterial(UUID materialID);
		static Ref<Material> GetMaterial(MaterialType type, UUID materialID);
		static void StoreMaterial(Ref<Material> material);

		static Ref<Material> GetMaterialByName(const std::string& name);
		static Ref<Material> GetMaterialByName(MaterialType type, const std::string& name);

		static void ClearMaterials();

		static void DeleteMaterial(MaterialType type, const std::string& materialName);

		static const std::unordered_map<MaterialType, std::vector<Ref<Material>>>& GetMaterialList() { return s_Materials; }


//		Sampler
		static std::string SamplerTypeToString(SamplerType type);
		static SamplerType GetSamplerTypeByName(const std::string& samplerName);

		static void AddSampler(SamplerType type, Ref<Vulkan::ImageSampler> sampler);

		static Ref<Vulkan::ImageSampler> GetSampler(SamplerType type);

		static std::unordered_map<SamplerType, Ref<Vulkan::ImageSampler>> GetSamplerMap() { return s_Samplers; }

//		Shader
		static void LoadShader(ShaderType type, const std::string& shaderPath);

		static Ref<Vulkan::Shader> GetShader(ShaderType type);

//		Texture
		static void AddTexture(Ref<Vulkan::Texture2D> texture);
		static bool FindTexture(const std::string& textureName);

		static Ref<Vulkan::Texture2D> GetTexture(UUID textureID);

		static std::unordered_map<UUID, Ref<Vulkan::Texture2D>>& GetTextureContainer() { return m_Textures; }

		static void ClearTextures();



	private:

		inline static std::unordered_map<MaterialType, std::vector<Ref<Material>>> s_Materials;

		inline static std::unordered_map<SamplerType, Ref<Vulkan::ImageSampler>> s_Samplers;

		inline static std::unordered_map<ShaderType, Ref<Vulkan::Shader>> s_Shaders;

		inline static std::unordered_map<UUID, Ref<Vulkan::Texture2D>> m_Textures;

	};



}
