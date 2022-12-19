#pragma once

#include "Device.h"
#include "Renderer.h"

#include <vulkan/vulkan.h>


namespace Wingnut
{

	enum class BufferType
	{
		Vertex,
		Index,
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

}
