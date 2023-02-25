#include "wingnut_pch.h"
#include "Material.h"

#include "Assets/TextureStore.h"

#include "Renderer/Renderer.h"


namespace Wingnut
{

	Ref<Material> Material::Create(const ObjMaterial& objMaterial, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler)
	{
		return CreateRef<Material>(objMaterial, shader, sampler);
	}

	Ref<Material> Material::Create(const std::string& name, Ref<Vulkan::Shader> shader)
	{
		return CreateRef<Material>(name, shader);
	}

	Ref<Material> Material::Create(const std::string& name, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler)
	{
		return CreateRef<Material>(name, shader, sampler);
	}

	Ref<Material> Material::Create(const std::string& name, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler, const MaterialData& materialData)
	{
		return CreateRef<Material>(name, shader, sampler, materialData);
	}


	Material::Material(const ObjMaterial& objMaterial, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler)
		: m_Name(objMaterial.MaterialName), m_Shader(shader)
	{
		if (objMaterial.HasDiffuseTexture)
		{
			Ref<Vulkan::Texture2D> texture = Vulkan::Texture2D::Create(objMaterial.DiffuseTexture, Vulkan::TextureFormat::R8G8B8A8_Normalized, true);
			TextureStore::AddTexture(texture);

			m_MaterialData.UseAlbedoTexture = true;
			m_MaterialData.AlbedoTexture = texture;

			CreateDescriptor(texture, shader, sampler);
		}
		else
		{
			m_Descriptor = Renderer::GetContext()->GetRendererData().DefaultTextureDescriptor;
		}
	}

	Material::Material(const std::string& name, Ref<Vulkan::Shader> shader)
		: m_Name(name), m_Shader(shader)
	{
		m_MaterialData.UseAlbedoTexture = true;
		m_MaterialData.AlbedoTexture = Renderer::GetContext()->GetRendererData().DefaultTexture;
		m_Descriptor = Renderer::GetContext()->GetRendererData().DefaultTextureDescriptor;
	}

	Material::Material(const std::string& name, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler)
		: m_Name(name), m_Shader(shader)
	{
		CreateDescriptor(shader, sampler);
	}

	Material::Material(const std::string& name, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler, const MaterialData& materialData)
		: m_MaterialData(materialData), m_Name(name), m_Shader(shader)
	{
		CreateDescriptor(shader, sampler);
	}

	Material::~Material()
	{
		Release();
	}

	void Material::Release()
	{
/*		if (m_MaterialData.Texture != nullptr)
		{
			m_MaterialData.Texture->Release();
			m_MaterialData.Texture = nullptr;
		}
*/	}

	void Material::CreateDescriptor(Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler)
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		m_Descriptor = Vulkan::Descriptor::Create(rendererData.Device, shader, sampler, TextureDescriptor, AlbedoTextureBinding, m_MaterialData.AlbedoTexture);

		TextureStore::AddDescriptor(m_MaterialData.AlbedoTexture->GetTextureID(), m_Descriptor);
	}

	void Material::CreateDescriptor(Ref<Vulkan::Texture2D> texture, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler)
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		m_Descriptor = Vulkan::Descriptor::Create(rendererData.Device, shader, sampler, TextureDescriptor, AlbedoTextureBinding, texture);

		TextureStore::AddDescriptor(texture->GetTextureID(), m_Descriptor);
	}

	void Material::SetSamplerType(SamplerType type)
	{
		if (type == m_SamplerType)
		{
			return;
		}

		m_SamplerType = type;

		CreateDescriptor(m_MaterialData.AlbedoTexture, m_Shader, SamplerStore::GetSampler(m_SamplerType));

		TextureStore::SetDescriptor(m_MaterialData.AlbedoTexture->GetTextureID(), m_Descriptor);
//		TextureStore::GetDescriptor(m_MaterialData.Texture->GetTextureID())->SetSampler(SamplerStore::GetSampler(m_SamplerType));
	}

}
