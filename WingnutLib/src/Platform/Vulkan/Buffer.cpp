#include "wingnut_pch.h"
#include "Buffer.h"



namespace Wingnut
{

	namespace Vulkan
	{

		void CreateBuffer(Ref<Device> device, VkBuffer& buffer, VkDeviceMemory& deviceMemory, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryFlags)
		{
			VkBufferCreateInfo bufferCreateInfo = {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.size = size;
			bufferCreateInfo.usage = usageFlags;
			bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateBuffer(device->GetDevice(), &bufferCreateInfo, nullptr, &buffer) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Renderer] Unable to create staging buffer");
				return;
			}

			LOG_CORE_TRACE("[Renderer] Created staging buffer");

			VkMemoryRequirements memoryRequirements;
			vkGetBufferMemoryRequirements(device->GetDevice(), buffer, &memoryRequirements);

			VkMemoryAllocateInfo memoryAllocateInfo = {};
			memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memoryAllocateInfo.allocationSize = memoryRequirements.size;
			memoryAllocateInfo.memoryTypeIndex = device->FindMemoryType(memoryRequirements.memoryTypeBits, memoryFlags);

			if (vkAllocateMemory(device->GetDevice(), &memoryAllocateInfo, nullptr, &deviceMemory) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Renderer] Unable to allocate buffer memory");
				return;
			}

			LOG_CORE_TRACE("[Renderer] Allocated buffer memory");

			vkBindBufferMemory(device->GetDevice(), buffer, deviceMemory, 0);
		}

		void CopyBuffer(Ref<Device> device, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize)
		{
			auto& rendererData = Renderer::GetContext()->GetRendererData();

			VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
			commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferAllocateInfo.commandPool = rendererData.TransferCommandPool->GetCommandPool();
			commandBufferAllocateInfo.commandBufferCount = 1;

			VkCommandBuffer commandBuffer = nullptr;
			vkAllocateCommandBuffers(device->GetDevice(), &commandBufferAllocateInfo, &commandBuffer);

			VkCommandBufferBeginInfo commandBufferBeginInfo = {};
			commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = 0;
			copyRegion.size = bufferSize;
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			vkQueueSubmit(device->GetQueue(QueueType::Graphics), 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(device->GetQueue(QueueType::Graphics));

			vkFreeCommandBuffers(device->GetDevice(), rendererData.TransferCommandPool->GetCommandPool(), 1, &commandBuffer);
		}

		VertexBuffer::VertexBuffer(Ref<Device> device, const std::vector<Vertex>& vertexList)
			: m_Device(device)
		{
			VkDeviceSize bufferSize = sizeof(Vertex) * (uint32_t)vertexList.size();

			VkBuffer stagingBuffer = nullptr;
			VkDeviceMemory stagingBufferMemory = nullptr;

			// Create staging buffer
			CreateBuffer(m_Device, stagingBuffer, stagingBufferMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			void* data = nullptr;
			vkMapMemory(m_Device->GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
			memcpy(data, vertexList.data(), (size_t)bufferSize);
			vkUnmapMemory(m_Device->GetDevice(), stagingBufferMemory);


			// Create vertex buffer

			CreateBuffer(m_Device, m_Buffer, m_BufferMemory, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			// Copy data
			CopyBuffer(m_Device, stagingBuffer, m_Buffer, bufferSize);

			vkDestroyBuffer(m_Device->GetDevice(), stagingBuffer, nullptr);
			vkFreeMemory(m_Device->GetDevice(), stagingBufferMemory, nullptr);
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


		////////////////////////////////////////////

		IndexBuffer::IndexBuffer(Ref<Device> device, const std::vector<uint32_t>& indexList)
			: m_Device(device)
		{
			m_IndexCount = (uint32_t)indexList.size();

			VkDeviceSize bufferSize = sizeof(uint32_t) * (uint32_t)indexList.size();

			VkBuffer stagingBuffer = nullptr;
			VkDeviceMemory stagingBufferMemory = nullptr;

			// Create staging buffer
			CreateBuffer(m_Device, stagingBuffer, stagingBufferMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			void* data = nullptr;
			vkMapMemory(m_Device->GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
			memcpy(data, indexList.data(), (size_t)bufferSize);
			vkUnmapMemory(m_Device->GetDevice(), stagingBufferMemory);


			// Create vertex buffer

			CreateBuffer(m_Device, m_Buffer, m_BufferMemory, bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			// Copy data
			CopyBuffer(m_Device, stagingBuffer, m_Buffer, bufferSize);

			vkDestroyBuffer(m_Device->GetDevice(), stagingBuffer, nullptr);
			vkFreeMemory(m_Device->GetDevice(), stagingBufferMemory, nullptr);
		}

		IndexBuffer::~IndexBuffer()
		{
			Release();
		}

		void IndexBuffer::Release()
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

		void IndexBuffer::Bind(Ref<CommandBuffer> commandBuffer, Ref<Pipeline> pipeline)
		{
			vkCmdBindPipeline(commandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

			vkCmdBindIndexBuffer(commandBuffer->GetCommandBuffer(), m_Buffer, 0, VK_INDEX_TYPE_UINT32);
		}

	}

}
