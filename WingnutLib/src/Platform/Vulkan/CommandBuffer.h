#pragma once

#include "CommandPool.h"
#include "Device.h"

#include <vulkan/vulkan.h>


namespace Wingnut
{

	namespace Vulkan
	{

		class CommandBuffer
		{
		public:
			CommandBuffer(Ref<Device> m_Device, Ref<CommandPool> commandPool);
			~CommandBuffer();

			void Release();

			void BeginRecording();
			void EndRecording();

			VkCommandBuffer GetCommandBuffer() { return m_CommandBuffer; }

		private:
			VkCommandBuffer m_CommandBuffer = nullptr;

			Ref<CommandPool> m_CommandPool = nullptr;
			Ref<Device> m_Device = nullptr;

		};


	}
}
