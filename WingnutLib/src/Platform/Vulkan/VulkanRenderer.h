#pragma once

#include "Renderer/Renderer.h"

#include <vulkan/vulkan.h>


namespace Wingnut
{

	class VulkanRenderer : public Renderer
	{
	public:
		VulkanRenderer();
		virtual ~VulkanRenderer();

		virtual RendererData& GetRendererData() override;


	private:
		void Create();

		bool CreateInstance();

		std::vector<std::string> FindInstanceLayers();
		std::vector<std::string> FindInstanceExtensions();

	private:
		VkInstance m_Instance = nullptr;

	};


}
