#pragma once

#include "Core/Timestep.h"

#include "Renderer/ImGuiRenderer.h"

#include "Platform/Vulkan/Texture.h"

#include "ECS/ECS.h"


namespace Wingnut
{


	class ImGuiContext
	{
	public:
		static Ref<ImGuiContext> Create();

		ImGuiContext();
		~ImGuiContext();

		void Release();

		void NewFrame(Timestep ts);
		void Render();

		static ImGuiContext& Get() { return *s_Instance; }

	private:
		inline static ImGuiContext* s_Instance = nullptr;

		Ref<ImGuiRenderer> m_Renderer = nullptr;

		Ref<Vulkan::UniformBuffer> m_CameraBuffer = nullptr;
		Ref<Vulkan::Descriptor> m_CameraDescriptor = nullptr;

		Ref<Vulkan::Texture2D> m_AtlasTexture = nullptr;
		Ref<Vulkan::Descriptor> m_AtlasDescriptor = nullptr;

		Ref<ECS::Registry> m_EntityRegistry;
		UUID m_ImGuiEntity = 0;

		uint32_t m_Width;
		uint32_t m_Height;
	};


}
