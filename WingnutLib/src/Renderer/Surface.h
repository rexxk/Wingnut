#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>


namespace Wingnut
{


	class Surface
	{
	public:
		Surface(VkInstance instance, void* windowHandle);
		virtual ~Surface();

		void Release();

		void* GetSurface() { return m_Surface; }


	private:
		void Create(void* windowHandle);

	private:
		VkSurfaceKHR m_Surface = nullptr;

		VkInstance m_Instance = nullptr;
	};


}
