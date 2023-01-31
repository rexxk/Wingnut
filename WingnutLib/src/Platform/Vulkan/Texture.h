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
		};


		class Texture2D
		{
		public:
			Texture2D(const std::string& texturePath, TextureFormat format, Ref<ImageSampler> sampler);
			Texture2D(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* pixels, TextureFormat format, Ref<ImageSampler> sampler);
			~Texture2D();

			void Release();

			VkImageView GetImageView() { return m_Image->GetImageView(); }
			Ref<ImageSampler> GetSampler() { return m_Sampler; }

		private:
			void CreateTextureFromFile(const std::string& texturePath);
			void CreateTexture(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* pixels);

		private:
			Ref<Image> m_Image = nullptr;
			Ref<ImageSampler> m_Sampler = nullptr;

			TextureFormat m_Format;
//			VkFormat m_Format = VK_FORMAT_UNDEFINED;

			Ref<Device> m_Device = nullptr;
		};


	}

}
