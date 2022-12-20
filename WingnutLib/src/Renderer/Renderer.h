#pragma once

#include <glm/glm.hpp>

#include "Platform/Vulkan/RendererContext.h"


namespace Wingnut
{

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
	};


	class Renderer
	{
	public:
		Renderer(void* windowHandle);
		virtual ~Renderer();

		void Release();

		static void BeginScene();
		static void EndScene();

		static void Present();


		static Renderer& Get() { return *s_Instance; }

		static Ref<Vulkan::VulkanContext> GetContext() { return s_Context; }

	private:
		inline static Renderer* s_Instance = nullptr;
		inline static Ref<Vulkan::VulkanContext> s_Context = nullptr;

	};


}
