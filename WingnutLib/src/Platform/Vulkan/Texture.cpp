#include "wingnut_pch.h"
#include "Texture.h"

#include "Buffer.h"

#include <stb_image.h>

#include <vulkan/vulkan.h>


namespace Wingnut
{
	namespace Vulkan
	{

		Texture2D::Texture2D(const std::string& texturePath, Ref<ImageSampler> sampler)
			: m_Sampler(sampler)
		{
			CreateTextureFromFile(texturePath);

			CreateImageView();
		}
		
		Texture2D::Texture2D(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* data, Ref<ImageSampler> sampler)
			: m_Sampler(sampler)
		{
			CreateTextureFromData(width, height, bitsPerPixel, data);

			CreateImageView();
		}

		Texture2D::~Texture2D()
		{
			Release();
		}

		void Texture2D::CreateTextureFromFile(const std::string& texturePath)
		{
			m_Device = Renderer::GetContext()->GetRendererData().Device;

			int width, height, channels;
			stbi_uc* pixels = stbi_load(texturePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

			if (pixels == nullptr)
			{
				LOG_CORE_ERROR("[Texture] Failed to load file {}", texturePath);
				return;
			}

//			m_Format = VK_FORMAT_R8G8B8A8_SRGB;
			m_Format = VK_FORMAT_R8G8B8A8_UNORM;

			VkDeviceSize size = width * height * 4;

			VkBuffer stagingBuffer = nullptr;
			VkDeviceMemory stagingBufferMemory = nullptr;

			Buffer::CreateBuffer(m_Device, stagingBuffer, stagingBufferMemory, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

			void* data;

			vkMapMemory(m_Device->GetDevice(), stagingBufferMemory, 0, size, 0, &data);
			memcpy(data, pixels, size);
			vkUnmapMemory(m_Device->GetDevice(), stagingBufferMemory);

			stbi_image_free(pixels);

			LOG_CORE_TRACE("[Texture] Loaded texture from file {}", texturePath);

			VkImageCreateInfo imageCreateInfo = {};
			imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
			imageCreateInfo.extent.width = (uint32_t)width;
			imageCreateInfo.extent.height = (uint32_t)height;
			imageCreateInfo.extent.depth = 1;

			imageCreateInfo.mipLevels = 1;
			imageCreateInfo.arrayLayers = 1;

			imageCreateInfo.format = m_Format;
			imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageCreateInfo.flags = 0;

			if (vkCreateImage(m_Device->GetDevice(), &imageCreateInfo, nullptr, &m_Image) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Texture] Unable to create texture image");
				return;
			}

			VkMemoryRequirements memoryRequirements;
			vkGetImageMemoryRequirements(m_Device->GetDevice(), m_Image, &memoryRequirements);

			VkMemoryAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocateInfo.allocationSize = memoryRequirements.size;
			allocateInfo.memoryTypeIndex = m_Device->FindMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			if (vkAllocateMemory(m_Device->GetDevice(), &allocateInfo, nullptr, &m_ImageMemory) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Texture] Unable to allocate texture image memory");
				return;
			}

			vkBindImageMemory(m_Device->GetDevice(), m_Image, m_ImageMemory, 0);

			Buffer::TransitionImageLayout(m_Device, m_Image, m_Format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			Buffer::CopyBufferToImage(m_Device, stagingBuffer, m_Image, (uint32_t)width, (uint32_t)height);
			Buffer::TransitionImageLayout(m_Device, m_Image, m_Format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			vkFreeMemory(m_Device->GetDevice(), stagingBufferMemory, nullptr);
			vkDestroyBuffer(m_Device->GetDevice(), stagingBuffer, nullptr);
		}

		void Texture2D::CreateTextureFromData(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* pixels)
		{
			m_Device = Renderer::GetContext()->GetRendererData().Device;

			if (pixels == nullptr)
			{
				LOG_CORE_ERROR("[Texture] No data for texture");
				return;
			}

//			m_Format = VK_FORMAT_R8G8B8A8_SRGB;
			m_Format = VK_FORMAT_R8G8B8A8_UNORM;

			VkDeviceSize size = width * height * 4;

			VkBuffer stagingBuffer = nullptr;
			VkDeviceMemory stagingBufferMemory = nullptr;

			Buffer::CreateBuffer(m_Device, stagingBuffer, stagingBufferMemory, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

			void* memoryAddress;

			vkMapMemory(m_Device->GetDevice(), stagingBufferMemory, 0, size, 0, &memoryAddress);
			memcpy(memoryAddress, pixels, size);
			vkUnmapMemory(m_Device->GetDevice(), stagingBufferMemory);

			LOG_CORE_TRACE("[Texture] Created texture");

			VkImageCreateInfo imageCreateInfo = {};
			imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
			imageCreateInfo.extent.width = (uint32_t)width;
			imageCreateInfo.extent.height = (uint32_t)height;
			imageCreateInfo.extent.depth = 1;

			imageCreateInfo.mipLevels = 1;
			imageCreateInfo.arrayLayers = 1;

			imageCreateInfo.format = m_Format;
			imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageCreateInfo.flags = 0;

			if (vkCreateImage(m_Device->GetDevice(), &imageCreateInfo, nullptr, &m_Image) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Texture] Unable to create texture image");
				return;
			}

			VkMemoryRequirements memoryRequirements;
			vkGetImageMemoryRequirements(m_Device->GetDevice(), m_Image, &memoryRequirements);

			VkMemoryAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocateInfo.allocationSize = memoryRequirements.size;
			allocateInfo.memoryTypeIndex = m_Device->FindMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			if (vkAllocateMemory(m_Device->GetDevice(), &allocateInfo, nullptr, &m_ImageMemory) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Texture] Unable to allocate texture image memory");
				return;
			}

			vkBindImageMemory(m_Device->GetDevice(), m_Image, m_ImageMemory, 0);

			Buffer::TransitionImageLayout(m_Device, m_Image, m_Format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			Buffer::CopyBufferToImage(m_Device, stagingBuffer, m_Image, (uint32_t)width, (uint32_t)height);
			Buffer::TransitionImageLayout(m_Device, m_Image, m_Format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			vkFreeMemory(m_Device->GetDevice(), stagingBufferMemory, nullptr);
			vkDestroyBuffer(m_Device->GetDevice(), stagingBuffer, nullptr);
		}


		void Texture2D::CreateImageView()
		{
			VkImageViewCreateInfo imageViewCreateInfo = {};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.image = m_Image;
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = m_Format;

			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_Device->GetDevice(), &imageViewCreateInfo, nullptr, &m_ImageView) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Texture] Unable to create texture image view");
				return;
			}
		}

		void Texture2D::Release()
		{

			if (m_ImageView != nullptr)
			{
				vkDestroyImageView(m_Device->GetDevice(), m_ImageView, nullptr);
				m_ImageView = nullptr;
			}

			if (m_Image != nullptr)
			{
				vkDestroyImage(m_Device->GetDevice(), m_Image, nullptr);
				m_Image = nullptr;
			}

			if (m_ImageMemory != nullptr)
			{
				vkFreeMemory(m_Device->GetDevice(), m_ImageMemory, nullptr);
				m_ImageMemory = nullptr;
			}

		}

	}
}
