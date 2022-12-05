#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

#include "Renderer/RendererSurface.h"


namespace Wingnut
{


	class VulkanSurface : public RendererSurface
	{
	public:
		VulkanSurface(VkInstance instance, void* windowHandle);
		virtual ~VulkanSurface();

		virtual void Release() override;

		VkSurfaceKHR GetSurface() { return m_Surface; }


	private:
		void Create(void* windowHandle);

	private:
		VkSurfaceKHR m_Surface = nullptr;

		VkInstance m_Instance = nullptr;
	};


}
