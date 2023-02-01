#pragma once


#include <vulkan/vulkan.h>

#include "Device.h"


namespace Wingnut
{

	namespace Vulkan
	{

		enum class CommandPoolType
		{
			Graphics,
			Transfer,
		};


		class CommandPool
		{
		public:
			static Ref<CommandPool> Create(Ref<Device> device, CommandPoolType type);

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
}
