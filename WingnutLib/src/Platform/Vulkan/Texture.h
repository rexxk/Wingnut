#pragma once

#include "Device.h"


namespace Wingnut
{

	namespace Vulkan
	{


		class Texture2D
		{
		public:
			Texture2D(const std::string& texturePath);
			~Texture2D();

			void Release();

			VkImageView GetImageView() { return m_ImageView; }
			VkSampler GetSampler() { return m_Sampler; }

		private:
			void CreateTexture(const std::string& texturePath);
			void CreateImageView();
			void CreateSampler();

		private:
			Ref<Device> m_Device = nullptr;

			VkImage m_Image = nullptr;
			VkDeviceMemory m_ImageMemory = nullptr;

			VkImageView m_ImageView = nullptr;

			VkSampler m_Sampler = nullptr;

			VkFormat m_Format = VK_FORMAT_UNDEFINED;
		};


	}

}
