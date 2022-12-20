#include "wingnut_pch.h"
#include "Surface.h"



namespace Wingnut
{

	namespace Vulkan
	{

		Surface::Surface(VkInstance instance, void* windowHandle)
			: m_Instance(instance)
		{
			Create(windowHandle);

		}

		Surface::~Surface()
		{
			Release();
		}

		void Surface::Release()
		{
			if (m_Surface != nullptr)
			{
				vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
				m_Surface = nullptr;
			}
		}

		void Surface::Create(void* windowHandle)
		{
			VkWin32SurfaceCreateInfoKHR createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			createInfo.hwnd = (HWND)windowHandle;
			createInfo.hinstance = GetModuleHandle(nullptr);

			if (vkCreateWin32SurfaceKHR(m_Instance, &createInfo, nullptr, &m_Surface) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Renderer] Unable to create win32 surface for Vulkan");
				return;
			}

			LOG_CORE_TRACE("[Renderer] Created win32 surface");
		}


	}
}
