#pragma once

#include "Device.h"


#include <vulkan/vulkan.h>



namespace Wingnut
{

	namespace Vulkan
	{


		enum class ImageType
		{
			DepthStencil,
		};


		class Image
		{
		public:
			Image(Ref<Device> device, ImageType type, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags memoryFlags, VkImageTiling tiling, VkImageAspectFlags aspectFlags);
			~Image();

			void Release();

//			VkImage GetImage() { return m_Image; }
			VkImageView GetImageView() { return m_ImageView; }

		private:
			void CreateImage(uint32_t width, uint32_t height, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags memoryFlags, VkImageTiling tiling);
			void CreateImageView(VkImageAspectFlags aspectFlags);

			void TransitionLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

		private:
			VkImage m_Image = nullptr;
			VkDeviceMemory m_ImageMemory = nullptr;
			VkImageView m_ImageView = nullptr;

			VkFormat m_Format;

			Ref<Device> m_Device = nullptr;

			ImageType m_Type;
		};



	}

}
