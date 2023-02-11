#pragma once


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
		static Ref<Material> Create(const std::string& name);
		static Ref<Material> Create(const std::string& name, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler);
		static Ref<Material> Create(const std::string& name, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler, const MaterialData& materialData);

		Material(const std::string& name);
		Material(const std::string& name, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler);
		Material(const std::string& name, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler, const MaterialData& materialData);
		~Material();

		void Release();

//		void SetTexture(Ref<Vulkan::Texture2D> texture) { m_Texture = texture; }

		void SetName(const std::string& name) { m_Name = name; }
		std::string& GetName() { return m_Name; }
		UUID GetID() const { return m_MaterialID; }

		void CreateDescriptor(Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler);
		Ref<Vulkan::Descriptor> GetDescriptor() { return m_Descriptor; }

		MaterialData& GetMaterialData() { return m_MaterialData; }

	private:
		MaterialData m_MaterialData;

		UUID m_MaterialID;

		Ref<Vulkan::Descriptor> m_Descriptor = nullptr;

		std::string m_Name;
	};



}
