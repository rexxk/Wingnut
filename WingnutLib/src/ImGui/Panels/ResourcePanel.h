#pragma once

#include "File/VirtualFileSystem.h"
#include "Platform/Vulkan/Texture.h"

namespace Wingnut
{

	class ResourcePanel
	{
	public:
		ResourcePanel();
		~ResourcePanel();

		void Release();

		void Draw();

	private:

		uint32_t m_HorizontalTextureCount = 1;

		FileSystemDirectory* m_ActiveDirectory = nullptr;

		bool m_FlipHorizontal = true;


		Ref<Vulkan::Texture2D> m_FileUpTexture = nullptr;
		Ref<Vulkan::Texture2D> m_FileModelTexture = nullptr;
		Ref<Vulkan::Texture2D> m_FileSceneTexture = nullptr;
		Ref<Vulkan::Texture2D> m_FileDirectoryTexture = nullptr;
	};


}
