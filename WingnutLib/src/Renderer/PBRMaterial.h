#pragma once

#include "Material.h"

#include "Platform/Vulkan/Buffer.h"

namespace Wingnut
{

	struct PBRMaterialProperties
	{
		glm::vec4 AlbedoColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

		float Metallic = 0.04f;
		float Roughness = 0.0f;
		float AmbientOcclusion = 1.0f;

		uint32_t UseAlbedoTexture = 0;
		uint32_t UseNormalMap = 0;
		uint32_t UseMetalnessMap = 0;
		uint32_t UseRoughnessMap = 0;
		uint32_t UseAmbientOcclusionMap = 0;
	};

	struct PBRMaterialData
	{
		PBRMaterialProperties Properties;

		MaterialTexture AlbedoTexture;
		MaterialTexture NormalMap;
		MaterialTexture MetalnessMap;
		MaterialTexture RoughnessMap;
		MaterialTexture AmbientOcclusionMap;
	};


	class PBRMaterial : public Material
	{
	public:
		static Ref<Material> Create(const ObjMaterial& objMaterial, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler);
		static Ref<Material> Create(const std::string& name, Ref<Vulkan::Shader> shader);

		PBRMaterial(const ObjMaterial& objMaterial, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler);
		PBRMaterial(const std::string& name, Ref<Vulkan::Shader> shader);
		virtual ~PBRMaterial();

		virtual void Release() override;

		virtual void Update() override;

		virtual void SetTexture(MaterialTextureType type, Ref<Vulkan::Texture2D> texture) override;

		virtual void CreateDescriptor(Ref<Vulkan::Shader> shader) override;

		void* GetMaterialData() { return (void*)&m_MaterialData; }

	private:
		void CreateUniformBuffer();

	private:
		PBRMaterialData m_MaterialData;

		SamplerType m_SamplerType = SamplerType::LinearRepeat;

		Ref<Vulkan::UniformBuffer> m_MaterialUB = nullptr;

		Ref<Vulkan::Shader> m_Shader = nullptr;

	};



}
