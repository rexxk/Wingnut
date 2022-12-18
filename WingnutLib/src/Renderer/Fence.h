#pragma once

#include "Device.h"

#include <vulkan/vulkan.h>


namespace Wingnut
{

	class Fence
	{
	public:
		Fence(Ref<Device> device);
		~Fence();

		void Release();

		void Wait(uint64_t timeout);
		void Reset();

		VkFence GetFence() { return m_Fence; }

	private:
		VkFence m_Fence = nullptr;

		Ref<Device> m_Device = nullptr;
	};

}
