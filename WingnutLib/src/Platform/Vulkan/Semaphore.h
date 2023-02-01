#pragma once

#include "Device.h"

#include <vulkan/vulkan.h>


namespace Wingnut
{
	
	namespace Vulkan
	{

		class Semaphore
		{
		public:
			static Ref<Semaphore> Create(Ref<Device> device);

			Semaphore(Ref<Device> device);
			~Semaphore();

			void Release();

			VkSemaphore GetSemaphore() { return m_Semaphore; }

		private:
			VkSemaphore m_Semaphore = nullptr;

			Ref<Device> m_Device = nullptr;
		};


	}
}
