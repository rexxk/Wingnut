#include "wingnut_pch.h"
#include "Material.h"

#include "Renderer/Renderer.h"


namespace Wingnut
{


	Ref<Material> Material::Create(const std::string& name, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler)
	{
		return CreateRef<Material>(name, shader, sampler);
	}

	Ref<Material> Material::Create(const std::string& name, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler, const MaterialData& materialData)
	{
		return CreateRef<Material>(name, shader, sampler, materialData);
	}


	Material::Material(const std::string& name, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler)
		: m_Name(name)
	{
		CreateDescriptor(shader, sampler);
	}

	Material::Material(const std::string& name, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler, const MaterialData& materialData)
		: m_MaterialData(materialData), m_Name(name)
	{
		CreateDescriptor(shader, sampler);
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
