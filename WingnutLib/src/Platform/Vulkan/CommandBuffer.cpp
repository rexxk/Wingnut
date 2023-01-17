#include "wingnut_pch.h"
#include "CommandBuffer.h"


namespace Wingnut
{

	namespace Vulkan
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

		void CommandBuffer::BeginRecording()
		{
			VkCommandBufferBeginInfo commandBufferBeginInfo = {};
			commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(m_CommandBuffer, &commandBufferBeginInfo);
		}

		void CommandBuffer::EndRecording()
		{
			vkEndCommandBuffer(m_CommandBuffer);

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_CommandBuffer;

			vkQueueSubmit(m_Device->GetQueue(QueueType::Graphics), 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(m_Device->GetQueue(QueueType::Graphics));
		}

	}

}
