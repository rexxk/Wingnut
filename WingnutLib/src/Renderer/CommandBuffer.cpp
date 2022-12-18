#include "wingnut_pch.h"
#include "CommandBuffer.h"



namespace Wingnut
{

	CommandBuffer::CommandBuffer(Ref<Device> device, Ref<CommandPool> commandPool)
		: m_Device(device), m_CommandPool(commandPool)
	{
		VkCommandBufferAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.commandPool = m_CommandPool->GetCommandPool();
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(m_Device->GetDevice(), &allocateInfo, &m_CommandBuffer) != VK_SUCCESS)
		{
			LOG_CORE_ERROR("[Renderer] Unable to allocate command buffer");
			return;
		}

		LOG_CORE_TRACE("[Renderer] Allocated command buffer");
	}

	CommandBuffer::~CommandBuffer()
	{
		Release();
	}

	void CommandBuffer::Release()
	{
		if (m_CommandBuffer != nullptr)
		{
			vkFreeCommandBuffers(m_Device->GetDevice(), m_CommandPool->GetCommandPool(), 1, &m_CommandBuffer);
			m_CommandBuffer = nullptr;
		}
	}

}
