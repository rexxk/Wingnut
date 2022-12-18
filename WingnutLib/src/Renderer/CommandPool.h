#pragma once


#include <vulkan/vulkan.h>

#include "Device.h"


namespace Wingnut
{


	class CommandPool
	{
	public:
		CommandPool(Ref<Device> device);
		~CommandPool();

		void Release();

		VkCommandPool GetCommandPool() { return m_CommandPool; }

	private:
		void CreateCommandPool();

	private:
		VkCommandPool m_CommandPool;

		VkDevice m_Device;
	};


}
