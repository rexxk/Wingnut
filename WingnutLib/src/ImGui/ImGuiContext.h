#pragma once

#include "Renderer/ImGuiRenderer.h"

#include "Platform/Vulkan/Texture.h"

#include "ECS/ECS.h"


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
		Ref<Vulkan::UniformBuffer> m_CameraDescriptor = nullptr;

		Ref<ECS::Registry> m_EntityRegistry;
		UUID m_ImGuiEntity = 0;

		uint32_t m_Width;
		uint32_t m_Height;
	};


}
