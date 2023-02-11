#include "wingnut_pch.h"
#include "TextureStore.h"



namespace Wingnut
{


	void TextureStore::AddTexture(Ref<Vulkan::Texture2D> texture)
	{
		if (m_TextureContainer.find(texture->GetTextureID()) == m_TextureContainer.end())
		{
			m_TextureContainer[texture->GetTextureID()] = texture;
		}
	}

	void TextureStore::AddDescriptor(UUID textureID, Ref<Vulkan::Descriptor> descriptor)
	{
		if (m_DescriptorContainer.find(textureID) == m_DescriptorContainer.end())
		{
			m_DescriptorContainer[textureID] = descriptor;
		}
	}

	void TextureStore::AddTextureData(Ref<Vulkan::Texture2D> texture, Ref<Vulkan::Descriptor> descriptor)
	{
		m_TextureContainer[texture->GetTextureID()] = texture;
		m_DescriptorContainer[texture->GetTextureID()] = descriptor;
	}

	std::pair<Ref<Vulkan::Texture2D>, Ref<Vulkan::Descriptor>> TextureStore::GetTextureData(UUID textureID)
	{
		return std::make_pair(GetTexture(textureID), GetDescriptor(textureID));
	}

	Ref<Vulkan::Texture2D> TextureStore::GetTexture(UUID textureID)
	{
		if (m_TextureContainer.find(textureID) != m_TextureContainer.end())
		{
			return m_TextureContainer[textureID];
		}

		return nullptr;
	}

	Ref<Vulkan::Descriptor> TextureStore::GetDescriptor(UUID textureID)
	{
		if (m_DescriptorContainer.find(textureID) != m_DescriptorContainer.end())
		{
			return m_DescriptorContainer[textureID];
		}

		return nullptr;
	}

	void TextureStore::ClearTextures()
	{
		for (auto& texture : m_TextureContainer)
		{
			texture.second->Release();
		}

		m_TextureContainer.clear();
		m_DescriptorContainer.clear();
	}


}
