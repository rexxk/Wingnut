#pragma once

#include "Device.h"

#include "Image.h"

#include "Shader.h"


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
			static Ref<Texture2D> Create(const std::string& texturePath, TextureFormat format, bool flip = false, bool createDescriptor = false, bool systemFile = false);
			static Ref<Texture2D> Create(const std::string& texturePath, TextureFormat format, uint32_t width, uint32_t height, const char* data, uint32_t texelSize);
			static Ref<Texture2D> Create(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* pixels, TextureFormat format, bool createDescriptor = false, bool systemFile = false);

			Texture2D(const std::string& texturePath, TextureFormat format, bool flip, bool createDescriptor, bool systemFile);
			Texture2D(const std::string& texturePath, TextureFormat format, uint32_t width, uint32_t height, const char* data, uint32_t texelSize);
			Texture2D(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* pixels, TextureFormat format, bool createDescriptor, bool systemFile);
			~Texture2D();

			void Release();

			Ref<Image> GetImage() { return m_Image; }
			VkImageView GetImageView() { return m_Image->GetImageView(); }

			VkDescriptorSet GetDescriptor() { return m_Descriptor->GetDescriptor(); }

			UUID GetTextureID() const { return m_TextureID; }
			std::string GetTextureName() const { return m_TextureName; }
			std::string GetTexturePath() const { return m_TexturePath; }

		private:
			void CreateTextureFromFile(const std::string& texturePath, bool flip, bool createDescriptor, bool systemFile);
			void CreateTextureFromMemory(const std::string& texturePath, uint32_t width, uint32_t height, const char* data, uint32_t texelSize);
			void CreateTexture(uint32_t width, uint32_t height, uint32_t bitsPerPixel, void* pixels, bool createDescriptor);

			void CreateDescriptor();

		private:
			Ref<Image> m_Image = nullptr;
			Ref<Device> m_Device = nullptr;

			TextureFormat m_Format;

			UUID m_TextureID;
			std::string m_TextureName = "<texture>";

			std::string m_TexturePath = "";

			Ref<Descriptor> m_Descriptor = nullptr;
		};


	}

}
