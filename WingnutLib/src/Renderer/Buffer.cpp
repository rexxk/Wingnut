#include "wingnut_pch.h"
#include "Buffer.h"



namespace Wingnut
{

	VertexBuffer::VertexBuffer(Ref<Device> device, const std::vector<Vertex>& vertexList)
		: m_Device(device)
	{
		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = (uint32_t)vertexList.size() * sizeof(Vertex);
		bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(m_Device->GetDevice(), &bufferCreateInfo, nullptr, &m_Buffer) != VK_SUCCESS)
		{
			LOG_CORE_ERROR("[Renderer] Unable to create vertex buffer");
			return;
		}

		LOG_CORE_TRACE("[Renderer] Created vertex buffer");

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(m_Device->GetDevice(), m_Buffer, &memoryRequirements);

		VkMemoryAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = memoryRequirements.size;
		allocateInfo.memoryTypeIndex = memoryRequirements.memoryTypeBits;

		if (vkAllocateMemory(m_Device->GetDevice(), &allocateInfo, nullptr, &m_BufferMemory) != VK_SUCCESS)
		{
			LOG_CORE_ERROR("[Renderer] Unable to allocate vertex buffer memory");
			return;
		}

		LOG_CORE_TRACE("[Renderer] Allocated vertex buffer memory");

		vkBindBufferMemory(m_Device->GetDevice(), m_Buffer, m_BufferMemory, 0);

		// Set data
		void* data = nullptr;

		vkMapMemory(m_Device->GetDevice(), m_BufferMemory, 0, bufferCreateInfo.size, 0, &data);
		memcpy(data, vertexList.data(), (size_t)bufferCreateInfo.size);
		vkUnmapMemory(m_Device->GetDevice(), m_BufferMemory);

	}

	VertexBuffer::~VertexBuffer()
	{
		Release();
	}

	void VertexBuffer::Release()
	{
		if (m_Buffer != nullptr)
		{
			vkDestroyBuffer(m_Device->GetDevice(), m_Buffer, nullptr);
			m_Buffer = nullptr;
		}

		if (m_BufferMemory != nullptr)
		{
			vkFreeMemory(m_Device->GetDevice(), m_BufferMemory, nullptr);
			m_BufferMemory = nullptr;
		}
	}

	void VertexBuffer::Bind(Ref<CommandBuffer> commandBuffer, Ref<Pipeline> pipeline)
	{
		vkCmdBindPipeline(commandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

		VkBuffer vertexBuffers[] = { m_Buffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer->GetCommandBuffer(), 0, 1, vertexBuffers, offsets);
	}


}
