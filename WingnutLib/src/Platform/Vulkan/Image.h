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


		enum class ImageSamplerFilter
		{
			Linear,
			Nearest,
		};

		enum class ImageSamplerMode
		{
			ClampToBorder,
			ClampToEdge,
			MirroredRepeat,
			MirrorClampToEdge,
			Repeat,
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


		class ImageSampler
		{
		public:
			ImageSampler(Ref<Device> device, ImageSamplerFilter filter, ImageSamplerMode mode);
			~ImageSampler();

			void Release();

			VkSampler Sampler() { return m_Sampler; }

		private:
			void CreateSampler(ImageSamplerFilter filter, ImageSamplerMode mode);

		private:
			Ref<Device> m_Device = nullptr;

			VkSampler m_Sampler = nullptr;
		};


	}

}
