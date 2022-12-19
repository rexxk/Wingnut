#pragma once

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "Device.h"
#include "Fence.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Semaphore.h"
#include "Surface.h"
#include "Swapchain.h"

#include <glm/glm.hpp>

#include <vulkan/vulkan.h>


namespace Wingnut
{

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
	};


	struct RendererData
	{
		Ref<CommandBuffer> CommandBuffer = nullptr;
		Ref<CommandPool> CommandPool = nullptr;
		Ref<Device> Device = nullptr;
		Ref<Framebuffer> Framebuffer = nullptr;
		Ref<Pipeline> Pipeline = nullptr;
		Ref<RenderPass> RenderPass = nullptr;
		Ref<Surface> Surface = nullptr;
		Ref<Swapchain> Swapchain = nullptr;

		Ref<Fence> InFlightFence = nullptr;
		Ref<Semaphore> ImageAvailableSemaphore = nullptr;
		Ref<Semaphore> RenderFinishedSemaphore = nullptr;
	};


	class Renderer
	{
	public:
		Renderer(void* windowHandle);
		virtual ~Renderer();

		void ReleaseAll();

		static void BeginScene();
		static void EndScene();

		static void Present();

		static RendererData& GetRendererData();

		Renderer& Get() { return *s_Instance; }

	private:
		void Create(void* windowHandle);

		bool CreateInstance();

		std::vector<std::string> FindInstanceLayers();
		std::vector<std::string> FindInstanceExtensions();

	private:
		VkInstance m_Instance = nullptr;

		VkExtent2D m_CurrentExtent;

		inline static Renderer* s_Instance = nullptr;
	};


}
