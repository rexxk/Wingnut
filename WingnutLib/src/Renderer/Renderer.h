#pragma once

#include "Device.h"
#include "Surface.h"
#include "Swapchain.h"

#include <vulkan/vulkan.h>


namespace Wingnut
{

	struct RendererData
	{
		Ref<Device> Device;
		Ref<Surface> Surface;
		Ref<Swapchain> Swapchain;
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
