#pragma once

#include "Device.h"

#include "Image.h"



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
			static Ref<Texture2D> Create(const std::string& texturePath, TextureFormat format);
			static Ref<Texture2D> Create(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* pixels, TextureFormat format);

			Texture2D(const std::string& texturePath, TextureFormat format);
			Texture2D(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* pixels, TextureFormat format);
			~Texture2D();

			void Release();

			Ref<Image> GetImage() { return m_Image; }
			VkImageView GetImageView() { return m_Image->GetImageView(); }

			UUID GetTextureID() const { return m_TextureID; }
			std::string GetTextureName() const { return m_TextureName; }

		private:
			void CreateTextureFromFile(const std::string& texturePath);
			void CreateTexture(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* pixels);

		private:
			Ref<Image> m_Image = nullptr;
			Ref<Device> m_Device = nullptr;

			TextureFormat m_Format;

			UUID m_TextureID;
			std::string m_TextureName = "<texture>";
		};


	}

}
