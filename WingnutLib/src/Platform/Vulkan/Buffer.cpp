#include "wingnut_pch.h"
#include "Buffer.h"

#include "CommandBuffer.h"


namespace Wingnut
{

	namespace Vulkan
	{

		void Buffer::CreateBuffer(Ref<Device> device, VkBuffer& buffer, VkDeviceMemory& deviceMemory, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryFlags)
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

		void Buffer::CopyBuffer(Ref<Device> device, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize)
		{
			auto commandPool = Renderer::GetContext()->GetRendererData().TransferCommandPool;

			Ref<CommandBuffer> commandBuffer = CreateRef<CommandBuffer>(device, commandPool);

			commandBuffer->BeginRecording();

			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = 0;
			copyRegion.size = bufferSize;
			vkCmdCopyBuffer(commandBuffer->GetCommandBuffer(), srcBuffer, dstBuffer, 1, &copyRegion);

			commandBuffer->EndRecording();
		}

		void Buffer::TransitionImageLayout(Ref<Device> device, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
		{
			auto commandPool = Renderer::GetContext()->GetRendererData().TransferCommandPool;

			Ref<CommandBuffer> commandBuffer = CreateRef<CommandBuffer>(device, commandPool);

			commandBuffer->BeginRecording();

			VkPipelineStageFlags sourceStage;
			VkPipelineStageFlags destinationStage;

			VkImageMemoryBarrier memoryBarrier = {};
			memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			memoryBarrier.oldLayout = oldLayout;
			memoryBarrier.newLayout = newLayout;

			memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

			memoryBarrier.image = image;
			memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			memoryBarrier.subresourceRange.baseMipLevel = 0;
			memoryBarrier.subresourceRange.levelCount = 1;
			memoryBarrier.subresourceRange.baseArrayLayer = 0;
			memoryBarrier.subresourceRange.layerCount = 1;

			if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				memoryBarrier.srcAccessMask = 0;
				memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			{
				memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}

			vkCmdPipelineBarrier(commandBuffer->GetCommandBuffer(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &memoryBarrier);

			commandBuffer->EndRecording();
		}

		void Buffer::CopyBufferToImage(Ref<Device> device, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
		{
			auto commandPool = Renderer::GetContext()->GetRendererData().TransferCommandPool;

			Ref<CommandBuffer> commandBuffer = CreateRef<CommandBuffer>(device, commandPool);
			commandBuffer->BeginRecording();

			VkBufferImageCopy region = {};
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;

			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;

			region.imageOffset = { 0, 0, 0 };
			region.imageExtent = { width, height, 1 };

			vkCmdCopyBufferToImage(commandBuffer->GetCommandBuffer(), buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

			commandBuffer->EndRecording();
		}



		VertexBuffer::VertexBuffer(Ref<Device> device, const std::vector<Vertex>& vertexList)
			: m_Device(device)
		{
			VkDeviceSize bufferSize = sizeof(Vertex) * (uint32_t)vertexList.size();

			VkBuffer stagingBuffer = nullptr;
			VkDeviceMemory stagingBufferMemory = nullptr;

			// Create staging buffer
			Buffer::CreateBuffer(m_Device, stagingBuffer, stagingBufferMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			void* data = nullptr;
			vkMapMemory(m_Device->GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
			memcpy(data, vertexList.data(), (size_t)bufferSize);
			vkUnmapMemory(m_Device->GetDevice(), stagingBufferMemory);


			// Create vertex buffer

			Buffer::CreateBuffer(m_Device, m_Buffer, m_BufferMemory, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			// Copy data
			Buffer::CopyBuffer(m_Device, stagingBuffer, m_Buffer, bufferSize);

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


		////////////////////////////////////////////

		IndexBuffer::IndexBuffer(Ref<Device> device, const std::vector<uint32_t>& indexList)
			: m_Device(device)
		{
			m_IndexCount = (uint32_t)indexList.size();

			VkDeviceSize bufferSize = sizeof(uint32_t) * (uint32_t)indexList.size();

			VkBuffer stagingBuffer = nullptr;
			VkDeviceMemory stagingBufferMemory = nullptr;

			// Create staging buffer
			Buffer::CreateBuffer(m_Device, stagingBuffer, stagingBufferMemory, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			void* data = nullptr;
			vkMapMemory(m_Device->GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
			memcpy(data, indexList.data(), (size_t)bufferSize);
			vkUnmapMemory(m_Device->GetDevice(), stagingBufferMemory);


			// Create vertex buffer

			Buffer::CreateBuffer(m_Device, m_Buffer, m_BufferMemory, bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			// Copy data
			Buffer::CopyBuffer(m_Device, stagingBuffer, m_Buffer, bufferSize);

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

		/////////////////////////////////////

		UniformBuffer::UniformBuffer(Ref<Device> device, uint32_t uniformBufferObjectSize)
			: m_Device(device)
		{
			VkDeviceSize bufferSize = uniformBufferObjectSize;

			m_Frames = Renderer::GetRendererSettings().FramesInFlight;

			m_Buffers.resize(m_Frames);
			m_BuffersMemory.resize(m_Frames);
			m_MappedBuffers.resize(m_Frames);

			for (uint32_t i = 0; i < m_Frames; i++)
			{
				// Create uniform buffer
				Buffer::CreateBuffer(m_Device, m_Buffers[i], m_BuffersMemory[i], bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

				vkMapMemory(m_Device->GetDevice(), m_BuffersMemory[i], 0, bufferSize, 0, &m_MappedBuffers[i]);
			}
		}

		UniformBuffer::~UniformBuffer()
		{
			Release();
		}

		void UniformBuffer::Release()
		{
			for (uint32_t i = 0; i < m_Frames; i++)
			{

				if (m_Buffers[i] != nullptr)
				{
					vkDestroyBuffer(m_Device->GetDevice(), m_Buffers[i], nullptr);
					m_Buffers[i] = nullptr;
				}

				if (m_BuffersMemory[i] != nullptr)
				{
					vkFreeMemory(m_Device->GetDevice(), m_BuffersMemory[i], nullptr);
					m_BuffersMemory[i] = nullptr;
				}
			}
		}

		void UniformBuffer::Update(void* uniformBufferObject, uint32_t uniformBufferObjectSize, uint32_t currentImageInFlight)
		{
			memcpy(m_MappedBuffers[currentImageInFlight], uniformBufferObject, uniformBufferObjectSize);
		}

	}

}
