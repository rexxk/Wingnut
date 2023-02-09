#pragma once


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
		static Ref<Material> Create();
		static Ref<Material> Create(const MaterialData& materialData);

		Material();
		Material(const MaterialData& materialData);
		~Material();

		void Release();

//		void SetTexture(Ref<Vulkan::Texture2D> texture) { m_Texture = texture; }

		void CreateDescriptor(Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler);
		Ref<Vulkan::Descriptor> GetDescriptor() { return m_Descriptor; }

		MaterialData& GetMaterialData() { return m_MaterialData; }

	private:
		MaterialData m_MaterialData;

		Ref<Vulkan::Descriptor> m_Descriptor = nullptr;
	};



}
