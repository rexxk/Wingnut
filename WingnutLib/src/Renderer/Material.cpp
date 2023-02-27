#include "wingnut_pch.h"
#include "Material.h"

#include "Assets/TextureStore.h"

#include "Renderer/Renderer.h"

#include "Platform/Vulkan/Buffer.h"
#include "Platform/Vulkan/Shader.h"


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


	Material::Material(const ObjMaterial& objMaterial, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler)
		: m_Name(objMaterial.MaterialName), m_Shader(shader)
	{
		CreateUniformBuffer();

		if (objMaterial.HasDiffuseTexture)
		{
			m_MaterialData.AlbedoTexture.Texture = Vulkan::Texture2D::Create(objMaterial.DiffuseTexture, Vulkan::TextureFormat::R8G8B8A8_Normalized, true);
			TextureStore::AddTexture(m_MaterialData.AlbedoTexture.Texture);

			m_MaterialData.Properties.UseAlbedoTexture = true;
		}
		else
		{
			m_MaterialData.AlbedoTexture.Texture = Renderer::GetContext()->GetRendererData().DefaultTexture;
		}

		m_MaterialData.Sampler = SamplerStore::GetSampler(SamplerType::Default);

		CreateDescriptor(m_Shader, sampler);

		Update();
	}

	Material::Material(const std::string& name, Ref<Vulkan::Shader> shader)
		: m_Name(name), m_Shader(shader)
	{
		CreateUniformBuffer();

		m_MaterialData.Properties.AlbedoColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
		m_MaterialData.Properties.UseAlbedoTexture = true;

		m_MaterialData.AlbedoTexture.Texture = Renderer::GetContext()->GetRendererData().DefaultTexture;
		m_MaterialData.Sampler = SamplerStore::GetSampler(SamplerType::Default);

//		m_Descriptor = Vulkan::Descriptor::Create(Renderer::GetContext()->GetRendererData().Device, m_Shader, MaterialDescriptor);

		CreateDescriptor(shader, SamplerStore::GetSampler(SamplerType::Default));

//		m_Descriptor->SetBufferBinding(MaterialDataBinding, m_MaterialUB);
//		m_Descriptor->SetImageBinding(AlbedoTextureBinding, m_MaterialData.AlbedoTexture.Texture, SamplerStore::GetSampler(SamplerType::Default));

		Update();
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
*/	
	
		if (m_MaterialUB != nullptr)
		{
			m_MaterialUB->Release();
		}
	}

	void Material::Update()
	{
		m_MaterialUB->Update(&m_MaterialData.Properties, sizeof(MaterialProperties), Renderer::GetContext()->GetCurrentFrame());
	}

	void Material::CreateUniformBuffer()
	{
		m_MaterialUB = Vulkan::UniformBuffer::Create(Renderer::GetContext()->GetRendererData().Device, sizeof(MaterialProperties));
	}

	void Material::CreateDescriptor(Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler)
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		m_Descriptor = Vulkan::Descriptor::Create(rendererData.Device, shader, MaterialDescriptor);

		m_Descriptor->SetBufferBinding(MaterialDataBinding, m_MaterialUB);
		m_Descriptor->SetImageBinding(AlbedoTextureBinding, m_MaterialData.AlbedoTexture.Texture, sampler);

		m_Descriptor->UpdateBindings();
	}

	void Material::SetTexture(MaterialType type, Ref<Vulkan::Texture2D> texture)
	{
		switch (type)
		{
			case MaterialType::AlbedoTexture:
			{
				m_MaterialData.AlbedoTexture.Texture = texture;
				m_Descriptor->SetImageBinding(AlbedoTextureBinding, m_MaterialData.AlbedoTexture.Texture, m_MaterialData.Sampler);

				break;
			}
		}


		m_Descriptor->UpdateBindings();

	}

	void Material::SetSamplerType(SamplerType type)
	{
		if (type == m_SamplerType)
		{
			return;
		}

		m_SamplerType = type;

		m_MaterialData.Sampler = SamplerStore::GetSampler(type);

//		TextureStore::SetDescriptor(m_MaterialData.AlbedoTexture.Texture->GetTextureID(), m_Descriptor);
//		TextureStore::GetDescriptor(m_MaterialData.Texture->GetTextureID())->SetSampler(SamplerStore::GetSampler(m_SamplerType));
	}

}
