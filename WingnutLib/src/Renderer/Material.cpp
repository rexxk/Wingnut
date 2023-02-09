#include "wingnut_pch.h"
#include "Material.h"

#include "Renderer/Renderer.h"


namespace Wingnut
{


	Ref<Material> Material::Create()
	{
		return CreateRef<Material>();
	}

	Ref<Material> Material::Create(const MaterialData& materialData)
	{
		return CreateRef<Material>(materialData);
	}


	Material::Material()
	{
//		m_Descriptor = Vulkan::Descriptor::Create()
	}

	Material::Material(const MaterialData& materialData)
		: m_MaterialData(materialData)
	{
//		CreateDescriptor();
	}

	Material::~Material()
	{
		Release();
	}

	void Material::Release()
	{
		if (m_MaterialData.Texture != nullptr)
		{
			m_MaterialData.Texture->Release();
			m_MaterialData.Texture = nullptr;
		}
	}

	void Material::CreateDescriptor(Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler)
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		m_Descriptor = Vulkan::Descriptor::Create(rendererData.Device, shader, sampler, MaterialDescriptor, 0, m_MaterialData.Texture);
	}



}
