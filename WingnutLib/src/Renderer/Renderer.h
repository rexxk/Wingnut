#pragma once

#include <glm/glm.hpp>

#include "Platform/Vulkan/Pipeline.h"
#include "Platform/Vulkan/RendererContext.h"


namespace Wingnut
{

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		glm::vec4 Color;
	};


	enum class PolygonFillType
	{
		Point,
		Wireframe,
		Solid,
	};

	struct RendererSettings
	{
		int FramesInFlight = 2;

		float LineWidth = 1.0f;

		PolygonFillType PolygonFill = PolygonFillType::Solid;
	};


	class Renderer
	{
	public:
		Renderer(void* windowHandle);
		virtual ~Renderer();

		void Release();

		static RendererSettings& GetRendererSettings() { return s_Instance->m_RendererSettings; }

		static void SubmitQueue();
		static void Present();

		static void BeginScene();
		static void EndScene();

		static void WaitForIdle();

		static Renderer& Get() { return *s_Instance; }
		static Ref<Vulkan::VulkanContext> GetContext() { return s_Context; }

	private:
		inline static Renderer* s_Instance = nullptr;
		inline static Ref<Vulkan::VulkanContext> s_Context = nullptr;

		RendererSettings m_RendererSettings;
	};


}
