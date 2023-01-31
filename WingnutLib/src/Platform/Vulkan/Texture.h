#pragma once

#include "Device.h"

#include "Image.h"



namespace Wingnut
{

	namespace Vulkan
	{


		class Texture2D
		{
		public:
			Texture2D(const std::string& texturePath, Ref<ImageSampler> sampler);
			Texture2D(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* pixels, Ref<ImageSampler> sampler);
			~Texture2D();

			void Release();

			VkImageView GetImageView() { return m_ImageView; }
			Ref<ImageSampler> GetSampler() { return m_Sampler; }

		private:
			void CreateTextureFromFile(const std::string& texturePath);
			void CreateTextureFromData(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* pixels);
			void CreateImageView();

		private:
			Ref<Device> m_Device = nullptr;

			VkImage m_Image = nullptr;
			VkDeviceMemory m_ImageMemory = nullptr;

			VkImageView m_ImageView = nullptr;

//			VkSampler m_Sampler = nullptr;

			Ref<ImageSampler> m_Sampler = nullptr;

			VkFormat m_Format = VK_FORMAT_UNDEFINED;
		};


	}

}
