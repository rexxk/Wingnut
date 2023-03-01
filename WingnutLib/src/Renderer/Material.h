#pragma once

#include "Assets/SamplerStore.h"

#include "DataImport/Obj/ObjLoader.h"

#include "Platform/Vulkan/Image.h"
#include "Platform/Vulkan/Shader.h"
#include "Platform/Vulkan/Texture.h"


namespace Wingnut
{


	struct MaterialTexture
	{
		Ref<Vulkan::Texture2D> Texture;
	};

	struct MaterialProperties
	{
		glm::vec4 AlbedoColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
		uint32_t UseAlbedoTexture = 0;
		uint32_t UseNormalMap = 0;
		float Metallic = 0.04f;
		float Roughness = 0.0f;
	};

	struct MaterialData
	{
		MaterialProperties Properties;

		MaterialTexture AlbedoTexture;
		MaterialTexture NormalMap;

		Ref<Vulkan::ImageSampler> Sampler;
	};

	enum class MaterialType
	{
		AlbedoTexture,
		NormalMap,
	};


	class Material
	{
	public:
		static Ref<Material> Create(const ObjMaterial& objMaterial, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler);
		static Ref<Material> Create(const std::string& name, Ref<Vulkan::Shader> shader);

		Material(const ObjMaterial& objMaterial, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler);
		Material(const std::string& name, Ref<Vulkan::Shader> shader);
		~Material();

		void Release();

		void Update();

		void SetTexture(MaterialType type, Ref<Vulkan::Texture2D> texture);

		void SetName(const std::string& name) { m_Name = name; }
		std::string& GetName() { return m_Name; }
		UUID GetID() const { return m_MaterialID; }

		void SetSamplerType(SamplerType type);
		SamplerType GetSamplerType() const { return m_SamplerType; }

		void CreateDescriptor(Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler);
		Ref<Vulkan::Descriptor> GetDescriptor() { return m_Descriptor; }

		MaterialData& GetMaterialData() { return m_MaterialData; }

	private:
		void CreateUniformBuffer();

	private:
		MaterialData m_MaterialData;

		UUID m_MaterialID;
		SamplerType m_SamplerType = SamplerType::LinearRepeat;

		Ref<Vulkan::Descriptor> m_Descriptor = nullptr;
		Ref<Vulkan::Shader> m_Shader = nullptr;
		Ref<Vulkan::UniformBuffer> m_MaterialUB = nullptr;

		std::string m_Name;
	};



}
