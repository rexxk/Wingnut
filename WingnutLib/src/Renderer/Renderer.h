#pragma once

#include "CommandPool.h"
#include "Device.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Surface.h"
#include "Swapchain.h"

#include <vulkan/vulkan.h>


namespace Wingnut
{

	struct RendererData
	{
		Ref<CommandPool> CommandPool = nullptr;
		Ref<Device> Device = nullptr;
		Ref<Framebuffer> Framebuffer = nullptr;
		Ref<Pipeline> Pipeline = nullptr;
		Ref<RenderPass> RenderPass = nullptr;
		Ref<Surface> Surface = nullptr;
		Ref<Swapchain> Swapchain = nullptr;
	};


	class Renderer
	{
	public:
		Renderer(void* windowHandle);
		virtual ~Renderer();

		RendererData& GetRendererData();


	private:
		void Create(void* windowHandle);

		bool CreateInstance();

		std::vector<std::string> FindInstanceLayers();
		std::vector<std::string> FindInstanceExtensions();

	private:
		VkInstance m_Instance = nullptr;

	};


}
