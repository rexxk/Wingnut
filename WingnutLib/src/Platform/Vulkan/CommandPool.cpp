#include "wingnut_pch.h"
#include "CommandPool.h"



namespace Wingnut
{

	namespace Vulkan
	{

		CommandPool::CommandPool(Ref<Device> device, CommandPoolType type)
			: m_Device(device->GetDevice()), m_Type(type)
		{
			CreateCommandPool();
		}

		CommandPool::~CommandPool()
		{
			Release();
		}

		void CommandPool::Release()
		{
			if (m_CommandPool != nullptr)
			{
				vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
				m_CommandPool = nullptr;
			}
		}

		void CommandPool::CreateCommandPool()
		{
			VkCommandPoolCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

			switch (m_Type)
			{
			case CommandPoolType::Graphics:
			{
				createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
				break;
			}
			case CommandPoolType::Transfer:
			{
				createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
				break;
			}
			}


			if (vkCreateCommandPool(m_Device, &createInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Renderer] Failed to create command pool");
				return;
			}

			LOG_CORE_TRACE("[Renderer] Command pool created");
		}

	}
}
