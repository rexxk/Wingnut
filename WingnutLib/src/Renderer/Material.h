#pragma once

#include "Assets/SamplerStore.h"

#include "DataImport/Obj/ObjLoader.h"

#include "Platform/Vulkan/Image.h"
#include "Platform/Vulkan/Shader.h"
#include "Platform/Vulkan/Texture.h"


namespace Wingnut
{

	struct MaterialData
	{
		Ref<Vulkan::Texture2D> Texture;
	};


	class Material
	{
	public:
		static Ref<Material> Create(const ObjMaterial& objMaterial, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler);
		static Ref<Material> Create(const std::string& name);
		static Ref<Material> Create(const std::string& name, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler);
		static Ref<Material> Create(const std::string& name, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler, const MaterialData& materialData);

		Material(const ObjMaterial& objMaterial, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler);
		Material(const std::string& name);
		Material(const std::string& name, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler);
		Material(const std::string& name, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler, const MaterialData& materialData);
		~Material();

		void Release();

//		void SetTexture(Ref<Vulkan::Texture2D> texture) { m_Texture = texture; }

		void SetName(const std::string& name) { m_Name = name; }
		std::string& GetName() { return m_Name; }
		UUID GetID() const { return m_MaterialID; }

		void SetSamplerType(SamplerType type);
		SamplerType GetSamplerType() const { return m_SamplerType; }

		void CreateDescriptor(Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler);
		void CreateDescriptor(Ref<Vulkan::Texture2D> texture, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler);
		Ref<Vulkan::Descriptor> GetDescriptor() { return m_Descriptor; }
		void SetDescriptor(Ref<Vulkan::Descriptor> descriptor) { m_Descriptor = descriptor; }

		MaterialData& GetMaterialData() { return m_MaterialData; }

	private:
		MaterialData m_MaterialData;

		UUID m_MaterialID;
		SamplerType m_SamplerType = SamplerType::LinearRepeat;

		Ref<Vulkan::Descriptor> m_Descriptor = nullptr;
		Ref<Vulkan::Shader> m_Shader = nullptr;

		std::string m_Name;
	};



}
