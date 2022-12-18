#pragma once

#include "Device.h"

#include <vulkan/vulkan.h>


namespace Wingnut
{

	class Semaphore
	{
	public:
		Semaphore(Ref<Device> device);
		~Semaphore();

		void Release();

		VkSemaphore GetSemaphore() { return m_Semaphore; }

	private:
		VkSemaphore m_Semaphore = nullptr;

		Ref<Device> m_Device = nullptr;
	};


}
