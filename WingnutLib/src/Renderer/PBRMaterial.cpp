#include "wingnut_pch.h"
#include "PBRMaterial.h"

#include "Assets/ResourceManager.h"

#include "File/VirtualFileSystem.h"


namespace Wingnut
{

	Ref<Material> PBRMaterial::Create(const ImportMaterial& objMaterial, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler)
	{
		return CreateRef<PBRMaterial>(objMaterial, shader, sampler);
	}

	Ref<Material> PBRMaterial::Create(const std::string& name, Ref<Vulkan::Shader> shader, SamplerType samplerType)
	{
		return CreateRef<PBRMaterial>(name, shader, samplerType);
	}

	PBRMaterial::PBRMaterial(UUID materialID, const std::string& materialName, Ref<Vulkan::Shader> shader, SamplerType samplerType)
		: Material(materialName, shader)
	{
		m_MaterialID = materialID;

		m_SamplerType = samplerType;
	}


	PBRMaterial::PBRMaterial(const ImportMaterial& objMaterial, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler)
		: Material(objMaterial.MaterialName, shader)
	{
		CreateUniformBuffer();

		m_Sampler = sampler;

		Ref<Vulkan::Texture2D> defaultTexture = Renderer::GetContext()->GetRendererData().DefaultTexture;

		m_MaterialData.AlbedoTexture.Texture = defaultTexture;
		m_MaterialData.NormalMap.Texture = defaultTexture;
		m_MaterialData.MetalnessMap.Texture = defaultTexture;
		m_MaterialData.RoughnessMap.Texture = defaultTexture;
		m_MaterialData.AmbientOcclusionMap.Texture = defaultTexture;

		if (objMaterial.HasDiffuseTexture)
		{
			bool flipTexture = true;

			if (objMaterial.DiffuseTexture.Scale.y == -1)
			{
				flipTexture = false;
			}

			Ref<Vulkan::Texture2D> texture = ResourceManager::GetTextureByName(objMaterial.DiffuseTexture.TextureName);

			if (texture == nullptr)
			{
				m_MaterialData.AlbedoTexture.Texture = Vulkan::Texture2D::Create(objMaterial.DiffuseTexture.TextureName, Vulkan::TextureFormat::R8G8B8A8_Normalized, flipTexture, true);
			}
			else
			{
				m_MaterialData.AlbedoTexture.Texture = texture;
			}

			m_MaterialData.Properties.UseAlbedoTexture = true;
		}

		if (objMaterial.HasNormalMap)
		{
			bool flipTexture = true;

			if (objMaterial.NormalMap.Scale.y == -1)
			{
				flipTexture = false;
			}

			Ref<Vulkan::Texture2D> texture = ResourceManager::GetTextureByName(objMaterial.NormalMap.TextureName);

			if (texture == nullptr)
			{
				m_MaterialData.NormalMap.Texture = Vulkan::Texture2D::Create(objMaterial.NormalMap.TextureName, Vulkan::TextureFormat::R8G8B8A8_Normalized, flipTexture, true);
			}
			else
			{
				m_MaterialData.NormalMap.Texture = texture;
			}

			m_MaterialData.Properties.UseNormalMap = true;
		}

		if (objMaterial.HasMetalnessMap)
		{
			bool flipTexture = true;

			if (objMaterial.MetalnessMap.Scale.y == -1)
			{
				flipTexture = false;
			}

			Ref<Vulkan::Texture2D> texture = ResourceManager::GetTextureByName(objMaterial.MetalnessMap.TextureName);

			if (texture == nullptr)
			{
				m_MaterialData.MetalnessMap.Texture = Vulkan::Texture2D::Create(objMaterial.MetalnessMap.TextureName, Vulkan::TextureFormat::R8G8B8A8_Normalized, flipTexture, true);
			}
			else
			{
				m_MaterialData.MetalnessMap.Texture = texture;
			}

			m_MaterialData.Properties.UseMetalnessMap = true;
		}

		if (objMaterial.HasRoughnessMap)
		{
			bool flipTexture = true;

			if (objMaterial.RoughnessMap.Scale.y == -1)
			{
				flipTexture = false;
			}

			Ref<Vulkan::Texture2D> texture = ResourceManager::GetTextureByName(objMaterial.RoughnessMap.TextureName);

			if (texture == nullptr)
			{
				m_MaterialData.RoughnessMap.Texture = Vulkan::Texture2D::Create(objMaterial.RoughnessMap.TextureName, Vulkan::TextureFormat::R8G8B8A8_Normalized, flipTexture, true);
			}
			else
			{
				m_MaterialData.RoughnessMap.Texture = texture;
			}

			m_MaterialData.Properties.UseRoughnessMap = true;
		}

		if (objMaterial.HasAmbientOcclusionMap)
		{
			bool flipTexture = true;

			if (objMaterial.AmbientOcclusionMap.Scale.y == -1)
			{
				flipTexture = false;
			}

			Ref<Vulkan::Texture2D> texture = ResourceManager::GetTextureByName(objMaterial.RoughnessMap.TextureName);

			if (texture == nullptr)
			{
				m_MaterialData.AmbientOcclusionMap.Texture = Vulkan::Texture2D::Create(objMaterial.AmbientOcclusionMap.TextureName, Vulkan::TextureFormat::R8G8B8A8_Normalized, flipTexture, true);
			}
			else
			{
				m_MaterialData.AmbientOcclusionMap.Texture = texture;
			}

			m_MaterialData.Properties.UseAmbientOcclusionMap = true;
		}

		if (objMaterial.HasPBRValues)
		{
			m_MaterialData.Properties.Metallic = objMaterial.Metalness;
			m_MaterialData.Properties.Roughness = objMaterial.Roughness;
			m_MaterialData.Properties.AmbientOcclusion = objMaterial.AmbientOcclusion;
		}
		else
		{
			m_MaterialData.Properties.Metallic = 0.04f;
			m_MaterialData.Properties.Roughness = 1.0f;
			m_MaterialData.Properties.AmbientOcclusion = 1.0f;
		}

		m_MaterialData.Properties.AlbedoColor = glm::vec4(objMaterial.Diffuse, objMaterial.Transparency);

		m_MaterialData.AlbedoTexture.TextureName = m_MaterialData.AlbedoTexture.Texture->GetTextureName();
		m_MaterialData.NormalMap.TextureName = m_MaterialData.NormalMap.Texture->GetTextureName();
		m_MaterialData.MetalnessMap.TextureName = m_MaterialData.MetalnessMap.Texture->GetTextureName();
		m_MaterialData.RoughnessMap.TextureName = m_MaterialData.RoughnessMap.Texture->GetTextureName();
		m_MaterialData.AmbientOcclusionMap.TextureName = m_MaterialData.AmbientOcclusionMap.Texture->GetTextureName();

		CreateDescriptor(m_Shader);

		Update();
	}

	PBRMaterial::PBRMaterial(const std::string& name, Ref<Vulkan::Shader> shader, SamplerType samplerType)
		: Material(name, shader)
	{
		CreateUniformBuffer();

		m_SamplerType = samplerType;

		m_MaterialData.Properties.AlbedoColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
		m_MaterialData.Properties.UseAlbedoTexture = true;

		m_MaterialData.AlbedoTexture.Texture = Renderer::GetContext()->GetRendererData().DefaultTexture;
		m_MaterialData.AlbedoTexture.TextureName = m_MaterialData.AlbedoTexture.Texture->GetTextureName();
		m_MaterialData.NormalMap.Texture = Renderer::GetContext()->GetRendererData().DefaultTexture;
		m_MaterialData.NormalMap.TextureName = m_MaterialData.NormalMap.Texture->GetTextureName();
		m_MaterialData.MetalnessMap.Texture = Renderer::GetContext()->GetRendererData().DefaultTexture;
		m_MaterialData.MetalnessMap.TextureName = m_MaterialData.MetalnessMap.Texture->GetTextureName();
		m_MaterialData.RoughnessMap.Texture = Renderer::GetContext()->GetRendererData().DefaultTexture;
		m_MaterialData.RoughnessMap.TextureName = m_MaterialData.RoughnessMap.Texture->GetTextureName();
		m_MaterialData.AmbientOcclusionMap.Texture = Renderer::GetContext()->GetRendererData().DefaultTexture;
		m_MaterialData.AmbientOcclusionMap.TextureName = m_MaterialData.AmbientOcclusionMap.Texture->GetTextureName();

		CreateDescriptor(shader);

		Update();
	}

	PBRMaterial::~PBRMaterial()
	{
		Release();
	}

	void PBRMaterial::Release()
	{
		if (m_MaterialUB != nullptr)
		{
			m_MaterialUB->Release();
		}

//		LOG_CORE_TRACE("[Material] Releasing PBR material");

		if (m_MaterialData.AlbedoTexture.Texture != nullptr)
		{
			m_MaterialData.AlbedoTexture.Texture->Release();
		}

		if (m_MaterialData.NormalMap.Texture != nullptr)
		{
			m_MaterialData.NormalMap.Texture->Release();
		}

		if (m_MaterialData.MetalnessMap.Texture != nullptr)
		{
			m_MaterialData.MetalnessMap.Texture->Release();
		}

		if (m_MaterialData.RoughnessMap.Texture != nullptr)
		{
			m_MaterialData.RoughnessMap.Texture->Release();
		}

		if (m_MaterialData.AmbientOcclusionMap.Texture != nullptr)
		{
			m_MaterialData.AmbientOcclusionMap.Texture->Release();
		}

	}

	void PBRMaterial::SetupMaterial()
	{
		CreateUniformBuffer();

		if (m_MaterialData.AlbedoTexture.Texture == nullptr)
		{
			FileSystemItem* item = VirtualFileSystem::GetItem(m_MaterialData.AlbedoTexture.TextureName);

			if (item)
			{
				if (ResourceManager::FindTexture(m_MaterialData.AlbedoTexture.TextureName))
				{
					m_MaterialData.AlbedoTexture.Texture = ResourceManager::GetTextureByName(m_MaterialData.AlbedoTexture.TextureName);
				}
				else
				{
					Ref<Vulkan::Texture2D> texture = Vulkan::Texture2D::Create(m_MaterialData.AlbedoTexture.TextureName, Vulkan::TextureFormat::R8G8B8A8_Normalized, item->DataSize, 0, (const char*)item->Data.data(), 0);
					ResourceManager::AddTexture(texture);

					m_MaterialData.AlbedoTexture.Texture = texture;
				}

			}
		}

		if (m_MaterialData.NormalMap.Texture == nullptr)
		{
			FileSystemItem* item = VirtualFileSystem::GetItem(m_MaterialData.NormalMap.TextureName);

			if (item)
			{
				if (ResourceManager::FindTexture(m_MaterialData.NormalMap.TextureName))
				{
					m_MaterialData.NormalMap.Texture = ResourceManager::GetTextureByName(m_MaterialData.NormalMap.TextureName);
				}
				else
				{
					Ref<Vulkan::Texture2D> texture = Vulkan::Texture2D::Create(m_MaterialData.NormalMap.TextureName, Vulkan::TextureFormat::R8G8B8A8_Normalized, item->DataSize, 0, (const char*)item->Data.data(), 0);
					ResourceManager::AddTexture(texture);

					m_MaterialData.NormalMap.Texture = texture;
				}
			}
		}

		if (m_MaterialData.MetalnessMap.Texture == nullptr)
		{
			FileSystemItem* item = VirtualFileSystem::GetItem(m_MaterialData.MetalnessMap.TextureName);

			if (item)
			{
				if (ResourceManager::FindTexture(m_MaterialData.MetalnessMap.TextureName))
				{
					m_MaterialData.MetalnessMap.Texture = ResourceManager::GetTextureByName(m_MaterialData.MetalnessMap.TextureName);
				}
				else
				{
					Ref<Vulkan::Texture2D> texture = Vulkan::Texture2D::Create(m_MaterialData.MetalnessMap.TextureName, Vulkan::TextureFormat::R8G8B8A8_Normalized, item->DataSize, 0, (const char*)item->Data.data(), 0);
					ResourceManager::AddTexture(texture);

					m_MaterialData.MetalnessMap.Texture = texture;
				}
			}
		}

		if (m_MaterialData.RoughnessMap.Texture == nullptr)
		{
			FileSystemItem* item = VirtualFileSystem::GetItem(m_MaterialData.RoughnessMap.TextureName);

			if (item)
			{
				if (ResourceManager::FindTexture(m_MaterialData.RoughnessMap.TextureName))
				{
					m_MaterialData.RoughnessMap.Texture = ResourceManager::GetTextureByName(m_MaterialData.RoughnessMap.TextureName);
				}
				else
				{
					Ref<Vulkan::Texture2D> texture = Vulkan::Texture2D::Create(m_MaterialData.RoughnessMap.TextureName, Vulkan::TextureFormat::R8G8B8A8_Normalized, item->DataSize, 0, (const char*)item->Data.data(), 0);
					ResourceManager::AddTexture(texture);

					m_MaterialData.RoughnessMap.Texture = texture;
				}
			}
		}

		if (m_MaterialData.AmbientOcclusionMap.Texture == nullptr)
		{
			FileSystemItem* item = VirtualFileSystem::GetItem(m_MaterialData.AmbientOcclusionMap.TextureName);

			if (item)
			{
				if (ResourceManager::FindTexture(m_MaterialData.AmbientOcclusionMap.TextureName))
				{
					m_MaterialData.AmbientOcclusionMap.Texture = ResourceManager::GetTextureByName(m_MaterialData.AmbientOcclusionMap.TextureName);
				}
				else
				{
					Ref<Vulkan::Texture2D> texture = Vulkan::Texture2D::Create(m_MaterialData.AmbientOcclusionMap.TextureName, Vulkan::TextureFormat::R8G8B8A8_Normalized, item->DataSize, 0, (const char*)item->Data.data(), 0);
					ResourceManager::AddTexture(texture);

					m_MaterialData.AmbientOcclusionMap.Texture = texture;
				}
			}
		}

		CreateDescriptor(m_Shader);

		Update();
	}

	void PBRMaterial::Update()
	{
		m_MaterialUB->Update(&m_MaterialData.Properties, sizeof(PBRMaterialProperties), Renderer::GetContext()->GetCurrentFrame());
	}

	void PBRMaterial::CreateUniformBuffer()
	{
		m_MaterialUB = Vulkan::UniformBuffer::Create(Renderer::GetContext()->GetRendererData().Device, sizeof(PBRMaterialProperties));
	}

	void PBRMaterial::CreateDescriptor(Ref<Vulkan::Shader> shader)
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		m_Descriptor = Vulkan::Descriptor::Create(rendererData.Device, shader, MaterialDescriptor);

		if (m_Sampler == nullptr)
		{
			m_Sampler = ResourceManager::GetSampler(m_SamplerType);
		}

		m_Descriptor->SetBufferBinding(PBRMaterialDataBinding, m_MaterialUB);
		m_Descriptor->SetImageBinding(PBRAlbedoTextureBinding, m_MaterialData.AlbedoTexture.Texture, m_Sampler);
		m_Descriptor->SetImageBinding(PBRNormalMapBinding, m_MaterialData.NormalMap.Texture, m_Sampler);
		m_Descriptor->SetImageBinding(PBRMetalnessMapBinding, m_MaterialData.MetalnessMap.Texture, m_Sampler);
		m_Descriptor->SetImageBinding(PBRRoughnessMapBinding, m_MaterialData.RoughnessMap.Texture, m_Sampler);
		m_Descriptor->SetImageBinding(PBRAmbientOcclusionMapBinding, m_MaterialData.AmbientOcclusionMap.Texture, m_Sampler);

		m_Descriptor->UpdateBindings();
	}

	void PBRMaterial::SetTexture(MaterialTextureType type, Ref<Vulkan::Texture2D> texture)
	{
		switch (type)
		{
		case MaterialTextureType::AlbedoTexture:
		{
			m_MaterialData.AlbedoTexture.Texture = texture;
			m_MaterialData.AlbedoTexture.TextureName = texture->GetTextureName();
			m_Descriptor->SetImageBinding(PBRAlbedoTextureBinding, m_MaterialData.AlbedoTexture.Texture, m_Sampler);

			break;
		}

		case MaterialTextureType::NormalMap:
		{
			m_MaterialData.NormalMap.Texture = texture;
			m_MaterialData.NormalMap.TextureName = texture->GetTextureName();
			m_Descriptor->SetImageBinding(PBRNormalMapBinding, m_MaterialData.NormalMap.Texture, m_Sampler);

			break;
		}

		case MaterialTextureType::MetalnessMap:
		{
			m_MaterialData.MetalnessMap.Texture = texture;
			m_MaterialData.MetalnessMap.TextureName = texture->GetTextureName();
			m_Descriptor->SetImageBinding(PBRMetalnessMapBinding, m_MaterialData.MetalnessMap.Texture, m_Sampler);

			break;
		}

		case MaterialTextureType::RoughnessMap:
		{
			m_MaterialData.RoughnessMap.Texture = texture;
			m_MaterialData.RoughnessMap.TextureName = texture->GetTextureName();
			m_Descriptor->SetImageBinding(PBRRoughnessMapBinding, m_MaterialData.RoughnessMap.Texture, m_Sampler);

			break;
		}

		case MaterialTextureType::AmbientOcclusionMap:
		{
			m_MaterialData.AmbientOcclusionMap.Texture = texture;
			m_MaterialData.AmbientOcclusionMap.TextureName = texture->GetTextureName();
			m_Descriptor->SetImageBinding(PBRAmbientOcclusionMapBinding, m_MaterialData.AmbientOcclusionMap.Texture, m_Sampler);

			break;
		}
		}


		m_Descriptor->UpdateBindings();

	}


	void PBRMaterial::Serialize(Serializer& serializer)
	{
		serializer.Write<glm::vec4>((const char*)&m_MaterialData.Properties.AlbedoColor);
		serializer.Write<float>((const char*)&m_MaterialData.Properties.Metallic);
		serializer.Write<float>((const char*)&m_MaterialData.Properties.Roughness);
		serializer.Write<float>((const char*)&m_MaterialData.Properties.AmbientOcclusion);

		serializer.Write<uint32_t>((const char*)&m_MaterialData.Properties.UseAlbedoTexture);
		serializer.Write<uint32_t>((const char*)&m_MaterialData.Properties.UseNormalMap);
		serializer.Write<uint32_t>((const char*)&m_MaterialData.Properties.UseMetalnessMap);
		serializer.Write<uint32_t>((const char*)&m_MaterialData.Properties.UseRoughnessMap);
		serializer.Write<uint32_t>((const char*)&m_MaterialData.Properties.UseAmbientOcclusionMap);

		serializer.Write<std::string>((const char*)m_MaterialData.AlbedoTexture.TextureName.c_str(), (uint32_t)m_MaterialData.AlbedoTexture.TextureName.size());
		serializer.Write<std::string>((const char*)m_MaterialData.NormalMap.TextureName.c_str(), (uint32_t)m_MaterialData.NormalMap.TextureName.size());
		serializer.Write<std::string>((const char*)m_MaterialData.MetalnessMap.TextureName.c_str(), (uint32_t)m_MaterialData.MetalnessMap.TextureName.size());
		serializer.Write<std::string>((const char*)m_MaterialData.RoughnessMap.TextureName.c_str(), (uint32_t)m_MaterialData.RoughnessMap.TextureName.size());
		serializer.Write<std::string>((const char*)m_MaterialData.AmbientOcclusionMap.TextureName.c_str(), (uint32_t)m_MaterialData.AmbientOcclusionMap.TextureName.size());

	}

	void PBRMaterial::Deserialize(Deserializer& deserializer)
	{
		m_MaterialData.Properties.AlbedoColor = deserializer.Read<glm::vec4>();
		m_MaterialData.Properties.Metallic = deserializer.Read<float>();
		m_MaterialData.Properties.Roughness = deserializer.Read<float>();
		m_MaterialData.Properties.AmbientOcclusion = deserializer.Read<float>();

		m_MaterialData.Properties.UseAlbedoTexture = deserializer.Read<uint32_t>();
		m_MaterialData.Properties.UseNormalMap = deserializer.Read<uint32_t>();
		m_MaterialData.Properties.UseMetalnessMap = deserializer.Read<uint32_t>();
		m_MaterialData.Properties.UseRoughnessMap = deserializer.Read<uint32_t>();
		m_MaterialData.Properties.UseAmbientOcclusionMap = deserializer.Read<uint32_t>();

		m_MaterialData.AlbedoTexture.TextureName = deserializer.Read<std::string>();
		m_MaterialData.NormalMap.TextureName = deserializer.Read<std::string>();
		m_MaterialData.MetalnessMap.TextureName = deserializer.Read<std::string>();
		m_MaterialData.RoughnessMap.TextureName = deserializer.Read<std::string>();
		m_MaterialData.AmbientOcclusionMap.TextureName = deserializer.Read<std::string>();
	}

}
