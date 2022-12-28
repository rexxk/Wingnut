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
			VertexBuffer(Ref<Device> device, const std::vector<Vertex>& vertexList);
			~VertexBuffer();

			void Release();

			void Bind(Ref<CommandBuffer> commandBuffer, Ref<Pipeline> pipeline);

			VkBuffer GetBuffer() { return m_Buffer; }

		private:
			VkBuffer m_Buffer = nullptr;
			VkDeviceMemory m_BufferMemory = nullptr;

			Ref<Device> m_Device = nullptr;
		};

		class IndexBuffer
		{
		public:
			IndexBuffer(Ref<Device> device, const std::vector<uint32_t>& indexList);
			~IndexBuffer();

			void Release();

			void Bind(Ref<CommandBuffer> commandBuffer, Ref<Pipeline> pipeline);

			uint32_t IndexCount() { return m_IndexCount; }

			VkBuffer GetBuffer() { return m_Buffer; }

		private:
			VkBuffer m_Buffer = nullptr;
			VkDeviceMemory m_BufferMemory = nullptr;

			uint32_t m_IndexCount = 0;

			Ref<Device> m_Device = nullptr;
		};

		class UniformBuffer
		{
		public:
			UniformBuffer(Ref<Device> device, uint32_t uniformBufferObjectSize);
			~UniformBuffer();

			void Release();

			VkBuffer GetBuffer(uint32_t frame) { return m_Buffers[frame]; };

			void Update(void* uniformBufferObject, uint32_t uniformBufferObjectSize, uint32_t currentImageInFlight);

		private:
			std::vector<VkBuffer> m_Buffers;
			std::vector<VkDeviceMemory> m_BuffersMemory;

			std::vector<void*> m_MappedBuffers;

			uint32_t m_Frames;

			Ref<Device> m_Device = nullptr;
		};

	}
}
