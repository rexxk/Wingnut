#pragma once


#include <vulkan/vulkan.h>

#include "Device.h"


namespace Wingnut
{


	enum class CommandPoolType
	{
		Graphics,
		Transfer,
	};


	class CommandPool
	{
	public:
		CommandPool(Ref<Device> device, CommandPoolType type);
		~CommandPool();

		void Release();

		VkCommandPool GetCommandPool() { return m_CommandPool; }

	private:
		void CreateCommandPool();

	private:
		VkCommandPool m_CommandPool;

		VkDevice m_Device;

		CommandPoolType m_Type;
	};


}
