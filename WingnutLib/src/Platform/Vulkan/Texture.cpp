#include "wingnut_pch.h"
#include "Texture.h"

#include "Buffer.h"

#include <stb_image.h>

#include <vulkan/vulkan.h>


namespace Wingnut
{
	namespace Vulkan
	{
		VkFormat TextureFormatToVulkanFormat(TextureFormat format)
		{
			switch (format)
			{
				case TextureFormat::R8G8B8A8_Normalized: return VK_FORMAT_R8G8B8A8_UNORM;
				case TextureFormat::R8G8B8A8_SRGB: return VK_FORMAT_R8G8B8A8_SRGB;
			}

			return VK_FORMAT_UNDEFINED;
		}


		Ref<Texture2D> Texture2D::Create(const std::string& texturePath, TextureFormat format, Ref<ImageSampler> sampler)
		{
			return CreateRef<Texture2D>(texturePath, format, sampler);
		}

		Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* pixels, TextureFormat format, Ref<ImageSampler> sampler)
		{
			return CreateRef<Texture2D>(width, height, bitsPerPixel, pixels, format, sampler);
		}


		Texture2D::Texture2D(const std::string& texturePath, TextureFormat format, Ref<ImageSampler> sampler)
			: m_Sampler(sampler), m_Format(format)
		{
			CreateTextureFromFile(texturePath);
		}
		
		Texture2D::Texture2D(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* data, TextureFormat format, Ref<ImageSampler> sampler)
			: m_Sampler(sampler), m_Format(format)
		{
			CreateTexture(width, height, bitsPerPixel, data);
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

			CreateTexture((uint32_t)width, (uint32_t)height, (uint32_t)channels, (void*)pixels);

			stbi_image_free(pixels);
		}

		void Texture2D::CreateTexture(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* pixels)
		{
			m_Device = Renderer::GetContext()->GetRendererData().Device;

			if (pixels == nullptr)
			{
				LOG_CORE_ERROR("[Texture] No data for texture");
				return;
			}

			VkDeviceSize size = width * height * 4;

			VkBuffer stagingBuffer = nullptr;
			VkDeviceMemory stagingBufferMemory = nullptr;

			Buffer::CreateBuffer(m_Device, stagingBuffer, stagingBufferMemory, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

			void* memoryAddress;

			vkMapMemory(m_Device->GetDevice(), stagingBufferMemory, 0, size, 0, &memoryAddress);
			memcpy(memoryAddress, pixels, size);
			vkUnmapMemory(m_Device->GetDevice(), stagingBufferMemory);

			LOG_CORE_TRACE("[Texture] Created texture");

			m_Image = CreateRef<Image>(m_Device, ImageType::Texture2D, width, height, TextureFormatToVulkanFormat(m_Format), VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

			Buffer::TransitionImageLayout(m_Device, m_Image->GetImage(), TextureFormatToVulkanFormat(m_Format), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			Buffer::CopyBufferToImage(m_Device, stagingBuffer, m_Image->GetImage(), (uint32_t)width, (uint32_t)height);
			Buffer::TransitionImageLayout(m_Device, m_Image->GetImage(), TextureFormatToVulkanFormat(m_Format), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

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
