#include "wingnut_pch.h"
#include "CommandPool.h"



namespace Wingnut
{

	CommandPool::CommandPool(Ref<Device> device)
		: m_Device(device->GetDevice())
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
		createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;


		if (vkCreateCommandPool(m_Device, &createInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
		{
			LOG_CORE_ERROR("[Renderer] Failed to create command pool");
			return;
		}

		LOG_CORE_TRACE("[Renderer] Command pool created");
	}

}
