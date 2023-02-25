#pragma once


#include "Platform/Vulkan/Shader.h"
#include "Platform/Vulkan/Texture.h"


namespace Wingnut
{


	class TextureStore
	{
	public:

		static void AddTexture(Ref<Vulkan::Texture2D> texture);
		static void AddDescriptor(UUID textureID, Ref<Vulkan::Descriptor> descriptor);
		static void AddTextureData(Ref<Vulkan::Texture2D> texture, Ref<Vulkan::Descriptor> descriptor);

		static void SetDescriptor(UUID textureID, Ref<Vulkan::Descriptor> descriptor);

		static std::pair<Ref<Vulkan::Texture2D>, Ref<Vulkan::Descriptor>> GetTextureData(UUID textureID);
		static Ref<Vulkan::Texture2D> GetTexture(UUID textureID);
		static Ref<Vulkan::Descriptor> GetDescriptor(UUID textureID);

		static std::unordered_map<UUID, Ref<Vulkan::Texture2D>>& GetTextureContainer() { return m_TextureContainer; }
		static std::unordered_map<UUID, Ref<Vulkan::Descriptor>>& GetDescriptorContainer() { return m_DescriptorContainer; }

		static void ClearTextures();

	private:
		inline static std::unordered_map<UUID, Ref<Vulkan::Texture2D>> m_TextureContainer;
		inline static std::unordered_map<UUID, Ref<Vulkan::Descriptor>> m_DescriptorContainer;
	};



}
