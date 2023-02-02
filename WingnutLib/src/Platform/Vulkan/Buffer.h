#pragma once

#include "Device.h"

#include "Renderer/Renderer.h"

#include <vulkan/vulkan.h>


namespace Wingnut
{
	namespace Vulkan
	{

		enum class BufferType
		{
			Vertex,
			Index,
			Uniform,
		};


		class VertexBuffer
		{
		public:
			static Ref<VertexBuffer> Create(Ref<Device> device, const void* data, uint32_t size);

			VertexBuffer(Ref<Device> device, const void* data, uint32_t size);
			~VertexBuffer();

			void Release();

			void Resize(const void* data, uint32_t size);
			void SetData(const void* data, uint32_t size);

			VkBuffer GetBuffer() { return m_Buffer; }

		private:
			VkBuffer m_Buffer = nullptr;
			VkDeviceMemory m_BufferMemory = nullptr;

			VkDeviceSize m_AllocatedBufferSize = 0;

			Ref<Device> m_Device = nullptr;
		};

		class IndexBuffer
		{
		public:
			static Ref<IndexBuffer> Create(Ref<Device> device, const void* data, uint32_t size, uint32_t count);

			IndexBuffer(Ref<Device> device, const void* data, uint32_t size, uint32_t count);
			~IndexBuffer();

			void Release();

			uint32_t IndexCount() { return m_IndexCount; }

			void Resize(const void* data, uint32_t size, uint32_t count);
			void SetData(const void* data, uint32_t size, uint32_t count);

			VkBuffer GetBuffer() { return m_Buffer; }

		private:
			VkBuffer m_Buffer = nullptr;
			VkDeviceMemory m_BufferMemory = nullptr;

			uint32_t m_IndexCount = 0;

			VkDeviceSize m_AllocatedBufferSize = 0;

			Ref<Device> m_Device = nullptr;
		};

		class UniformBuffer
		{
		public:
			static Ref<UniformBuffer> Create(Ref<Device> device, uint32_t objectSize);

			UniformBuffer(Ref<Device> device, uint32_t uniformBufferObjectSize);
			~UniformBuffer();

			void Release();

			VkBuffer GetBuffer(uint32_t frame) { return m_Buffers[frame]; };
			VkDeviceSize GetBufferSize() { return m_BufferSize; }

			void Update(void* uniformBufferObject, uint32_t uniformBufferObjectSize, uint32_t currentImageInFlight);

		private:
			std::vector<VkBuffer> m_Buffers;
			std::vector<VkDeviceMemory> m_BuffersMemory;

			std::vector<void*> m_MappedBuffers;

			VkDeviceSize m_BufferSize;

			uint32_t m_Frames;

			Ref<Device> m_Device = nullptr;
		};

		class Buffer
		{
		public:
			static void CreateBuffer(Ref<Device> device, VkBuffer& buffer, VkDeviceMemory& deviceMemory, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryFlags);
			static void CopyBuffer(Ref<Device> device, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize);

			static void TransitionImageLayout(Ref<Device> device, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
			static void CopyBufferToImage(Ref<Device> device, VkBuffer buffer,  VkImage image, uint32_t width, uint32_t height);
		};
	}
}
