#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>


namespace Wingnut
{

	namespace Vulkan
	{

		class Surface
		{
		public:
			static Ref<Surface> Create(VkInstance instance, void* windowHandle);

			Surface(VkInstance instance, void* windowHandle);
			virtual ~Surface();

			void Release();

			void* GetSurface() { return m_Surface; }


		private:
			void CreateSurface(void* windowHandle);

		private:
			VkSurfaceKHR m_Surface = nullptr;

			VkInstance m_Instance = nullptr;
		};


	}
}
