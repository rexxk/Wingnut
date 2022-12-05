#pragma once

#include "Renderer/Renderer.h"

#include <vulkan/vulkan.h>


namespace Wingnut
{

	class VulkanRenderer : public Renderer
	{
	public:
		VulkanRenderer(void* windowHandle);
		virtual ~VulkanRenderer();

		virtual RendererData& GetRendererData() override;


	private:
		void Create(void* windowHandle);

		bool CreateInstance();

		std::vector<std::string> FindInstanceLayers();
		std::vector<std::string> FindInstanceExtensions();

	private:
		VkInstance m_Instance = nullptr;

	};


}
