#pragma once

#include "DataImport/Obj/ObjLoader.h"

#include "Platform/Vulkan/Image.h"
#include "Platform/Vulkan/Shader.h"
#include "Platform/Vulkan/Texture.h"


namespace Wingnut
{


	enum class MaterialType
	{
		UI,
		StaticPBR,
		DynamicPBR,
	};


	struct MaterialTexture
	{
		Ref<Vulkan::Texture2D> Texture;
	};

	enum class MaterialTextureType
	{
		ThumbnailTexture,
		AlbedoTexture,
		NormalMap,
		MetalnessMap,
		RoughnessMap,
		AmbientOcclusionMap,
	};


	class Material
	{
	public:
		static Ref<Material> Create(const ObjMaterial& objMaterial, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler);
		static Ref<Material> Create(const std::string& name, Ref<Vulkan::Shader> shader);

		Material(const ObjMaterial& objMaterial, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler);
		Material(const std::string& name, Ref<Vulkan::Shader> shader);
		virtual ~Material();

		virtual void Release();

		virtual void Update();

		virtual void SetTexture(MaterialTextureType type, Ref<Vulkan::Texture2D> texture);

		void SetName(const std::string& name) { m_Name = name; }
		std::string& GetName() { return m_Name; }
		UUID GetID() const { return m_MaterialID; }
		MaterialType GetType() const { return m_MaterialType; }

//		void SetSamplerType(SamplerType type);
		SamplerType GetSamplerType() const { return m_SamplerType; }

		void SetSampler(Ref<Vulkan::ImageSampler> sampler) { m_Sampler = sampler; }

		virtual void CreateDescriptor(Ref<Vulkan::Shader> shader);
		Ref<Vulkan::Descriptor> GetDescriptor() { return m_Descriptor; }

	protected:
		UUID m_MaterialID;
		MaterialType m_MaterialType = MaterialType::StaticPBR;

		SamplerType m_SamplerType = SamplerType::LinearRepeat;

		Ref<Vulkan::Descriptor> m_Descriptor = nullptr;
		Ref<Vulkan::Shader> m_Shader = nullptr;

		Ref<Vulkan::ImageSampler> m_Sampler = nullptr;

		std::string m_Name;
	};



}
