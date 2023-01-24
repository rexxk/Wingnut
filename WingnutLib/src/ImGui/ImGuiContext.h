#pragma once

#include "Renderer/ImGuiRenderer.h"

#include "Platform/Vulkan/Texture.h"


namespace Wingnut
{

	class ImGuiContext
	{
	public:
		ImGuiContext();
		~ImGuiContext();

		void Release();

		void NewFrame();
		void Render();


	private:
		Ref<ImGuiRenderer> m_Renderer = nullptr;

		Ref<Vulkan::Texture2D> m_AtlasTexture = nullptr;

		uint32_t m_Width;
		uint32_t m_Height;
	};


}
