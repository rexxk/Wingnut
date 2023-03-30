#include "wingnut_pch.h"
#include "Texture.h"

#include "Buffer.h"
#include "Shader.h"

#include "Assets/ResourceManager.h"

#include "File/VirtualFileSystem.h"

#include "Utils/StringUtils.h"

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
				case TextureFormat::RenderTarget: return VK_FORMAT_B8G8R8A8_UNORM;
			}

			return VK_FORMAT_UNDEFINED;
		}


		Ref<Texture2D> Texture2D::Create(const std::string& texturePath, TextureFormat format, bool flip, bool createDescriptor, bool systemFile)
		{
			return CreateRef<Texture2D>(texturePath, format, flip, createDescriptor, systemFile);
		}

		Ref<Texture2D> Texture2D::Create(const std::string& texturePath, TextureFormat format, uint32_t width, uint32_t height, const char* data, uint32_t texelSize)
		{
			return CreateRef<Texture2D>(texturePath, format, width, height, data, texelSize);
		}

		Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* pixels, TextureFormat format, bool createDescriptor, bool systemFile)
		{
			return CreateRef<Texture2D>(width, height, bitsPerPixel, pixels, format, createDescriptor, systemFile);
		}


		Texture2D::Texture2D(const std::string& texturePath, TextureFormat format, bool flip, bool createDescriptor, bool systemFile)
			: m_Format(format), m_TexturePath(ConvertFilePath(texturePath))
		{
			CreateTextureFromFile(texturePath, flip, createDescriptor, systemFile);
		}
		
		Texture2D::Texture2D(const std::string& texturePath, TextureFormat format, uint32_t width, uint32_t height, const char* data, uint32_t texelSize)
			: m_Format(format), m_TexturePath(ConvertFilePath(texturePath))
		{
			CreateTextureFromMemory(texturePath, width, height, data, texelSize);
		}

		Texture2D::Texture2D(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* data, TextureFormat format, bool createDescriptor, bool systemFile)
			: m_Format(format)
		{
			CreateTexture(width, height, bitsPerPixel, data, createDescriptor);
		}

		Texture2D::~Texture2D()
		{
			Release();
		}

		void Texture2D::CreateTextureFromFile(const std::string& texturePath, bool flip, bool createDescriptor, bool systemFile)
		{
			std::string textureFilename = ConvertFilePathToAssetPath(texturePath);

			if (textureFilename == "")
			{
				textureFilename = texturePath;
			}

			m_Device = Renderer::GetContext()->GetRendererData().Device;

//			VirtualFileSystem::AddFile(texturePath);
			VirtualFileSystem::LoadFileFromDisk(textureFilename, FileItemType::Texture, systemFile);

			stbi_set_flip_vertically_on_load(flip);

			int width, height, channels;
			stbi_uc* pixels = stbi_load(textureFilename.c_str(), &width, &height, &channels, STBI_rgb_alpha);

			if (pixels == nullptr)
			{
				LOG_CORE_ERROR("[Texture] Failed to load file {}", textureFilename);
				return;
			}

			m_TextureName = textureFilename;
			m_TexturePath = textureFilename;
//			m_TextureName = textureFilename.substr(textureFilename.find_last_of("/\\") + 1);

			LOG_CORE_TRACE("{} - {}", textureFilename, m_TextureName);

			CreateTexture((uint32_t)width, (uint32_t)height, (uint32_t)channels, (void*)pixels, createDescriptor);

			stbi_image_free(pixels);

			if (m_Descriptor != nullptr)
			{
				VirtualFileSystem::SetItemLink(textureFilename, (void*)m_Descriptor->GetDescriptor());
			}
		}

		void Texture2D::CreateTextureFromMemory(const std::string& texturePath, uint32_t width, uint32_t height, const char* data, uint32_t texelSize)
		{
//			std::string textureFilename = ConvertFilePathToAssetPath("assets/textures/" + texturePath);
			std::string textureFilename = ConvertFilePathToAssetPath(texturePath);
//			VirtualFileSystem::LoadFileFromDisk(textureFilename, FileItemType::Texture, false);

			uint32_t dataSize = 0;

			if (height == 0)
			{
				dataSize = width;
			}
			else
			{
				dataSize = width * height * texelSize;
			}

			std::vector<uint8_t> fileData(dataSize);
			memcpy(fileData.data(), data, dataSize);

			if (!VirtualFileSystem::FindFile(textureFilename))
			{
				VirtualFileSystem::AddFile(textureFilename, fileData, (uint32_t)fileData.size(), FileItemType::Texture);
			}

			m_TextureName = texturePath;

			stbi_set_flip_vertically_on_load(true);

			int textureWidth = 0;
			int textureHeight = 0;
			int channels = 0;

			stbi_uc* pixels = stbi_load_from_memory((const stbi_uc*)data, dataSize, &textureWidth, &textureHeight, &channels, 4);

			CreateTexture((uint32_t)textureWidth, (uint32_t)textureHeight, (uint32_t)4, pixels, true);

			stbi_image_free(pixels);

			if (m_Descriptor != nullptr)
			{
				VirtualFileSystem::SetItemLink(textureFilename, (void*)m_Descriptor->GetDescriptor());
			}
		}

		void Texture2D::CreateTexture(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* pixels, bool createDescriptor)
		{
			m_Device = Renderer::GetContext()->GetRendererData().Device;

			VkDeviceSize size = width * height * 4;

			VkBuffer stagingBuffer = nullptr;
			VkDeviceMemory stagingBufferMemory = nullptr;

			Buffer::CreateBuffer(m_Device, stagingBuffer, stagingBufferMemory, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

			if (pixels != nullptr)
			{
				void* memoryAddress;

				vkMapMemory(m_Device->GetDevice(), stagingBufferMemory, 0, size, 0, &memoryAddress);
				memcpy(memoryAddress, pixels, size);
				vkUnmapMemory(m_Device->GetDevice(), stagingBufferMemory);
			}


			LOG_CORE_TRACE("[Texture] Created texture");

			VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT;

			if (m_Format == TextureFormat::RenderTarget)
			{
				usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			}
			else
			{
				usageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			}

			m_Image = Image::Create(m_Device, ImageType::Texture2D, width, height, TextureFormatToVulkanFormat(m_Format), usageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

			if (m_Format != TextureFormat::RenderTarget)
			{
				Buffer::TransitionImageLayout(m_Device, m_Image->GetImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
				Buffer::CopyBufferToImage(m_Device, stagingBuffer, m_Image->GetImage(), (uint32_t)width, (uint32_t)height);
				Buffer::TransitionImageLayout(m_Device, m_Image->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			}

			vkFreeMemory(m_Device->GetDevice(), stagingBufferMemory, nullptr);
			vkDestroyBuffer(m_Device->GetDevice(), stagingBuffer, nullptr);

			if (createDescriptor)
			{
				CreateDescriptor();
			}

		}

		void Texture2D::Release()
		{

			if (m_Image)
			{
				m_Image->Release();
			}

		}

		void Texture2D::CreateDescriptor()
		{
			auto& rendererData = Renderer::GetContext()->GetRendererData();
			m_Descriptor = Vulkan::Descriptor::Create(rendererData.Device, ResourceManager::GetShader(ShaderType::ImGui), ImGuiTextureDescriptor);

			m_Descriptor->SetImageBinding(ImGuiTextureBinding, m_Image->GetImageView(), ResourceManager::GetSampler(SamplerType::Default));
			m_Descriptor->UpdateBindings();
		}

	}
}
