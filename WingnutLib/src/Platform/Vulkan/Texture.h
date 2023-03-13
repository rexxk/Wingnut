#pragma once

#include "Device.h"

#include "Image.h"

#include "Shader.h"


namespace Wingnut
{

	namespace Vulkan
	{

		enum class TextureFormat
		{
			R8G8B8A8_Normalized,
			R8G8B8A8_SRGB,
			RenderTarget,
		};


		class Texture2D
		{
		public:
			static Ref<Texture2D> Create(const std::string& texturePath, TextureFormat format, bool flip = false, bool createDescriptor = false);
			static Ref<Texture2D> Create(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* pixels, TextureFormat format, bool createDescriptor = false);

			Texture2D(const std::string& texturePath, TextureFormat format, bool createDescriptor, bool flip);
			Texture2D(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* pixels, TextureFormat format, bool createDescriptor);
			~Texture2D();

			void Release();

			Ref<Image> GetImage() { return m_Image; }
			VkImageView GetImageView() { return m_Image->GetImageView(); }

			VkDescriptorSet GetDescriptor() { return m_Descriptor->GetDescriptor(); }

			UUID GetTextureID() const { return m_TextureID; }
			std::string GetTextureName() const { return m_TextureName; }

		private:
			void CreateTextureFromFile(const std::string& texturePath, bool flip);
			void CreateTexture(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* pixels);

			void CreateDescriptor();

		private:
			Ref<Image> m_Image = nullptr;
			Ref<Device> m_Device = nullptr;

			TextureFormat m_Format;

			UUID m_TextureID;
			std::string m_TextureName = "<texture>";

			Ref<Descriptor> m_Descriptor = nullptr;
		};


	}

}
