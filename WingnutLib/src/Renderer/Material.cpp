#include "wingnut_pch.h"
#include "Material.h"

#include "Renderer/Renderer.h"


namespace Wingnut
{


	Ref<Material> Material::Create(const std::string& name)
	{
		return CreateRef<Material>(name);
	}

	Ref<Material> Material::Create(const std::string& name, const MaterialData& materialData)
	{
		return CreateRef<Material>(name, materialData);
	}


	Material::Material(const std::string& name)
		: m_Name(name)
	{
//		m_Descriptor = Vulkan::Descriptor::Create()
	}

	Material::Material(const std::string& name, const MaterialData& materialData)
		: m_MaterialData(materialData), m_Name(name)
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
