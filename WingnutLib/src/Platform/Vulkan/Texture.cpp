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
		}
		
		Texture2D::Texture2D(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* data, Ref<ImageSampler> sampler)
			: m_Sampler(sampler)
		{
			CreateTextureFromData(width, height, bitsPerPixel, data);
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

			m_Image = CreateRef<Image>(m_Device, ImageType::Texture2D, width, height, m_Format, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

			Buffer::TransitionImageLayout(m_Device, m_Image->GetImage(), m_Format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			Buffer::CopyBufferToImage(m_Device, stagingBuffer, m_Image->GetImage(), (uint32_t)width, (uint32_t)height);
			Buffer::TransitionImageLayout(m_Device, m_Image->GetImage(), m_Format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

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

			m_Image = CreateRef<Image>(m_Device, ImageType::Texture2D, width, height, m_Format, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

			Buffer::TransitionImageLayout(m_Device, m_Image->GetImage(), m_Format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			Buffer::CopyBufferToImage(m_Device, stagingBuffer, m_Image->GetImage(), (uint32_t)width, (uint32_t)height);
			Buffer::TransitionImageLayout(m_Device, m_Image->GetImage(), m_Format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			vkFreeMemory(m_Device->GetDevice(), stagingBufferMemory, nullptr);
			vkDestroyBuffer(m_Device->GetDevice(), stagingBuffer, nullptr);
		}

		void Texture2D::Release()
		{

			if (m_Image)
			{
				m_Image->Release();
			}

		}

	}
}
